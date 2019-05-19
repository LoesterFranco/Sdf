/*
* Copyright 2019 Rochus Keller <mailto:me@rochus-keller.ch>
*
* This file is part of the Sdf (Standard Delay Format) parser library.
*
* The following is the license that applies to this copy of the
* library. For a license to use the library under conditions
* other than those described here, please email to me@rochus-keller.ch.
*
* GNU General Public License Usage
* This file may be used under the terms of the GNU General Public
* License (GPL) versions 2.0 or 3.0 as published by the Free Software
* Foundation and appearing in the file LICENSE.GPL included in
* the packaging of this file. Please review the following information
* to ensure GNU General Public Licensing requirements will be met:
* http://www.fsf.org/licensing/licenses/info/GPLv2.html and
* http://www.gnu.org/copyleft/gpl.html.
*/

#include "SdfLexer.h"
#include "SdfErrors.h"
#include "SdfFileCache.h"
#include <QBuffer>
#include <QFile>
#include <QIODevice>
using namespace Sdf;

QHash<QByteArray,QByteArray> Lexer::d_symbols;

Lexer::Lexer(QObject *parent) : QObject(parent),
    d_lastToken(Tok_Invalid),d_lineNr(0),d_colNr(0),d_in(0),d_err(0),d_fcache(0),
    d_ignoreComments(true), d_packComments(true),d_real("\\d*\\.?\\d+(e[-+]?\\d+)?")
{

}

void Lexer::setStream(QIODevice* in, const QString& sourcePath)
{
    if( in == 0 )
        setStream( sourcePath );
    else
    {
        d_in = in;
        d_lineNr = 0;
        d_colNr = 0;
        d_sourcePath = sourcePath;
        d_lastToken = Tok_Invalid;
    }
}

bool Lexer::setStream(const QString& sourcePath)
{
    QIODevice* in = 0;

    if( d_fcache )
    {
        bool found;
        QByteArray content = d_fcache->getFile(sourcePath, &found );
        if( found )
        {
            QBuffer* buf = new QBuffer(this);
            buf->setData( content );
            buf->open(QIODevice::ReadOnly);
            in = buf;
        }
    }

    if( in == 0 )
    {
        QFile* file = new QFile(sourcePath, this);
        if( !file->open(QIODevice::ReadOnly) )
        {
            if( d_err )
            {
                d_err->error(Errors::Lexer, sourcePath, 0, 0,
                                 tr("cannot open file from path %1").arg(sourcePath) );
            }
            delete file;
            return false;
        }
        in = file;
    }
    // else
    setStream( in, sourcePath );
    return true;
}

Token Lexer::nextToken()
{
    Token t;
    if( !d_buffer.isEmpty() )
    {
        t = d_buffer.first();
        d_buffer.pop_front();
    }else
        t = nextTokenImp();
    if( t.d_type == Tok_Comment && d_ignoreComments )
        t = nextToken();
    return t;
}

Token Lexer::peekToken(quint8 lookAhead)
{
    Q_ASSERT( lookAhead > 0 );
    while( d_buffer.size() < lookAhead )
        d_buffer.push_back( nextTokenImp() );
    return d_buffer[ lookAhead - 1 ];
}

QList<Token> Lexer::tokens(const QString& code)
{
    return tokens( code.toLatin1() );
}

QList<Token> Lexer::tokens(const QByteArray& code, const QString& path)
{
    QBuffer in;
    in.setData( code );
    in.open(QIODevice::ReadOnly);
    setStream( &in, path );

    QList<Token> res;
    Token t = nextToken();
    while( t.isValid() )
    {
        res << t;
        t = nextToken();
    }
    return res;
}

QByteArray Lexer::getSymbol(const QByteArray& str)
{
    if( str.isEmpty() )
        return str;
    QByteArray& sym = d_symbols[str];
    if( sym.isEmpty() )
        sym = str;
    return sym;
}

static inline bool isCharacter(char ch, bool withDigit = true )
{
    return ( withDigit ? ::isalnum(ch) : ::isalpha(ch) ) || ch == '_' || ch == '$' || ch == '\\';
}

Token Lexer::nextTokenImp()
{
    if( d_in == 0 )
        return token(Tok_Eof);
    skipWhiteSpace();

    while( d_colNr >= d_line.size() )
    {
        if( d_in->atEnd() )
        {
            Token t = token( Tok_Eof, 0 );
            if( d_in->parent() == this )
                d_in->deleteLater();
            return t;
        }
        nextLine();
        skipWhiteSpace();
    }
    Q_ASSERT( d_colNr < d_line.size() );
    while( d_colNr < d_line.size() )
    {
        const char ch = quint8(d_line[d_colNr]);

        if( ch == '"' )
            return string();
        else if( isCharacter(ch) )
            return identOrNumber();
        // else
        int pos = d_colNr;
        TokenType tt = tokenTypeFromString(d_line,&pos);

        if( tt == Tok_Lcmt )
            return blockComment();
        else if( tt == Tok_2Slash )
            return lineComment();
        else if( tt == Tok_Invalid || pos == d_colNr )
            return token( Tok_Invalid, 1, QString("unexpected character '%1' %2").arg(char(ch)).arg(int(ch)).toUtf8() );
        else {
            const int len = pos - d_colNr;
            return token( tt, len, d_line.mid(d_colNr,len) );
        }
    }
    Q_ASSERT(false);
    return token(Tok_Invalid);
}

int Lexer::skipWhiteSpace()
{
    const int colNr = d_colNr;
    while( d_colNr < d_line.size() && ::isspace( d_line[d_colNr] ) )
        d_colNr++;
    return d_colNr - colNr;
}

void Lexer::nextLine()
{
    d_colNr = 0;
    d_lineNr++;
    d_line = d_in->readLine();

    if( d_line.endsWith("\r\n") )
        d_line.chop(2);
    else if( d_line.endsWith('\n') || d_line.endsWith('\r') || d_line.endsWith('\025') )
        d_line.chop(1);
}

int Lexer::lookAhead(int off) const
{
    if( int( d_colNr + off ) < d_line.size() )
    {
        return d_line[ d_colNr + off ];
    }else
        return 0;
}

Token Lexer::token(TokenType tt, int len, const QByteArray& val)
{
    QByteArray v = val;
    if( tt != Tok_Comment && tt != Tok_Invalid )
        v = getSymbol(v);
    Token t( tt, d_lineNr, d_colNr + 1, len, v );
    d_lastToken = t;
    d_colNr += len;
    t.d_sourcePath = d_sourcePath;
    if( tt == Tok_Invalid && d_err != 0 )
        d_err->error(Errors::Syntax, t.d_sourcePath, t.d_lineNr, t.d_colNr, t.d_val );
    return t;
}

static inline bool isOnlyDecimalDigit( const QByteArray& str )
{
    for( int i = 0; i < str.size(); i++ )
    {
        if( !::isdigit(str[i]) )
            return false;
    }
    return true;
}

Token Lexer::identOrNumber()
{
    // ButtonStates_OBUF\[0\]_inst)
    // 1999999991808.0:1999999991808.0:1999999991808.0)

    // TODO scalar_constant 'b0, 1'b1 etc. not yet supported

    QByteArray str = d_line.mid(d_colNr );
    if( d_real.indexIn( str ) == 0 )
    {
        const int off = d_real.matchedLength();
        const QByteArray num = d_line.mid(d_colNr, off );
        Q_ASSERT( !num.isEmpty() );
        if( isOnlyDecimalDigit(num) )
            return token( Tok_Int, off, num );
        else
            return token( Tok_Real, off, num );
    }

    int off = 1;
    while( true )
    {
        const char c = lookAhead(off);
        if( c == '\\' )
            off += 2;
        else if( !isCharacter(c) )
            break;
        else
            off++;
    }
    str = str.left( off );
    Q_ASSERT( !str.isEmpty() );

    int pos = 0;
    TokenType t = tokenTypeFromString( str, &pos );
    if( t != Tok_Invalid && pos != str.size() )
        t = Tok_Invalid;

    if( t != Tok_Invalid )
        return token( t, off );
    else
        return token( Tok_Ident, off, str );
}

void Lexer::parseComment( const QByteArray& str, int& pos, int& level )
{
    enum State { Idle, Lb, Star } state = Idle;
    while( pos < str.size() )
    {
        const char c = str[pos++];
        switch( state )
        {
        case Idle:
            if( c == '/')
                state = Lb;
            else if( c == '*' )
                state = Star;
            break;
        case Lb:
            if( c == '*' )
                level++;
            state = Idle;
            break;
        case Star:
            if( c == '/')
                level--;
            state = Idle;
            if( level <= 0 )
                return;
            break;
        }
    }
}

Token Lexer::blockComment()
{
    const int startLine = d_lineNr;
    const int startCol = d_colNr;


    int level = 0;
    int pos = d_colNr;
    parseComment( d_line, pos, level );
    QByteArray str = d_line.mid(d_colNr,pos-d_colNr);
    while( level > 0 && !d_in->atEnd() )
    {
        nextLine();
        pos = 0;
        parseComment( d_line, pos, level );
        if( !str.isEmpty() )
            str += '\n';
        str += d_line.mid(d_colNr,pos-d_colNr);
    }
    if( d_packComments && level > 0 && d_in->atEnd() )
    {
        d_colNr = d_line.size();
        Token t( Tok_Invalid, startLine, startCol + 1, str.size(), tr("non-terminated comment").toLatin1() );
        if( d_err )
            d_err->error(Errors::Syntax, t.d_sourcePath, t.d_lineNr, t.d_colNr, t.d_val );
        return t;
    }
    // Col + 1 weil wir immer bei Spalte 1 beginnen, nicht bei Spalte 0
    Token t( ( d_packComments ? Tok_Comment : Tok_Lcmt ), startLine, startCol + 1, str.size(), str );
    t.d_sourcePath = d_sourcePath;
    d_lastToken = t;
    d_colNr = pos;
    if( !d_packComments && level == 0 )
    {
        Token t(Tok_Rcmt,d_lineNr, pos - 2 + 1, 2 );
        t.d_sourcePath = d_sourcePath;
        d_lastToken = t;
        d_buffer.append( t );
    }
    return t;
}

Token Lexer::lineComment()
{
    return token( Tok_Comment, d_line.size() - d_colNr, d_line.mid(d_colNr + 2).trimmed() );
}

Token Lexer::string()
{
    int off = 1;
    while( true )
    {
        const char c = lookAhead(off);
        off++;
        if( c == '\\' )
        {
            // ignore
        }else if( c == '"' )
            break;
        if( c == 0 )
            return token( Tok_Invalid, off, "non-terminated string" );
    }
    const QByteArray str = d_line.mid(d_colNr, off );
    return token( Tok_Str, off, str );
}

