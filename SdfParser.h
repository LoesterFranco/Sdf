

// This file was automatically generated by Coco/R; don't modify it.
#if !defined(Sdf_COCO_PARSER_H__)
#define Sdf_COCO_PARSER_H__

#include <QStack>
#include <Sdf/SdfSynTree.h>


#include "SdfLexer.h"

namespace Sdf {



class Parser {
private:
	enum {
		_EOF=0,
		_T_Literals_=1,
		_T_Bang=2,
		_T_BangEq=3,
		_T_Bang2Eq=4,
		_T_Percent=5,
		_T_Amp=6,
		_T_2Amp=7,
		_T_Lpar=8,
		_T_Rpar=9,
		_T_Star=10,
		_T_Rcmt=11,
		_T_Plus=12,
		_T_Comma=13,
		_T_Minus=14,
		_T_Dot=15,
		_T_Slash=16,
		_T_Lcmt=17,
		_T_2Slash=18,
		_T_Colon=19,
		_T_Lt=20,
		_T_2Lt=21,
		_T_Leq=22,
		_T_2Eq=23,
		_T_3Eq=24,
		_T_Gt=25,
		_T_Geq=26,
		_T_2Gt=27,
		_T_Qmark=28,
		_T_Lbrack=29,
		_T_Rbrack=30,
		_T_Hat=31,
		_T_HatTilde=32,
		_T_Bar=33,
		_T_2Bar=34,
		_T_Tilde=35,
		_T_TildeAmp=36,
		_T_TildeHat=37,
		_T_TildeBar=38,
		_T_Keywords_=39,
		_T_ABSOLUTE=40,
		_T_ARRIVAL=41,
		_T_BIDIRECTSKEW=42,
		_T_CCOND=43,
		_T_CELL=44,
		_T_CELLTYPE=45,
		_T_COND=46,
		_T_CONDELSE=47,
		_T_DATE=48,
		_T_DELAY=49,
		_T_DELAYFILE=50,
		_T_DEPARTURE=51,
		_T_DESIGN=52,
		_T_DEVICE=53,
		_T_DIFF=54,
		_T_DIVIDER=55,
		_T_EXCEPTION=56,
		_T_HOLD=57,
		_T_INCREMENT=58,
		_T_INSTANCE=59,
		_T_INTERCONNECT=60,
		_T_IOPATH=61,
		_T_LABEL=62,
		_T_NAME=63,
		_T_NETDELAY=64,
		_T_NOCHANGE=65,
		_T_PATHCONSTRAINT=66,
		_T_PATHPULSE=67,
		_T_PATHPULSEPERCENT=68,
		_T_PERIOD=69,
		_T_PERIODCONSTRAINT=70,
		_T_PORT=71,
		_T_PROCESS=72,
		_T_PROGRAM=73,
		_T_RECOVERY=74,
		_T_RECREM=75,
		_T_REMOVAL=76,
		_T_RETAIN=77,
		_T_SCOND=78,
		_T_SDFVERSION=79,
		_T_SETUP=80,
		_T_SETUPHOLD=81,
		_T_SKEW=82,
		_T_SKEWCONSTRAINT=83,
		_T_SLACK=84,
		_T_SUM=85,
		_T_TEMPERATURE=86,
		_T_TIMESCALE=87,
		_T_TIMINGCHECK=88,
		_T_TIMINGENV=89,
		_T_VENDOR=90,
		_T_VERSION=91,
		_T_VOLTAGE=92,
		_T_WAVEFORM=93,
		_T_WIDTH=94,
		_T_negedge=95,
		_T_posedge=96,
		_T_Specials_=97,
		_T_Ident=98,
		_T_Int=99,
		_T_Str=100,
		_T_Real=101,
		_T_Comment=102,
		_T_Eof=103,
		_T_MaxToken_=104
	};
	int maxT;

	int errDist;
	int minErrDist;

	void SynErr(int n, const char* ctx = 0);
	void Get();
	void Expect(int n, const char* ctx = 0);
	bool StartOf(int s);
	void ExpectWeak(int n, int follow);
	bool WeakSeparator(int n, int syFol, int repFol);
    void SynErr(const QString& sourcePath, int line, int col, int n, Sdf::Errors* err, const char* ctx, const QString& = QString() );

public:
	Sdf::Lexer *scanner;
	Sdf::Errors  *errors;

	Sdf::Token d_cur;
	Sdf::Token d_next;
	QList<Sdf::Token> d_comments;
	struct TokDummy
	{
		int kind;
	};
	TokDummy d_dummy;
	TokDummy *la;			// lookahead token
	
	int peek( quint8 la = 1 );

    void RunParser()
    {
        d_stack.push(&d_root);
        Parse();
        d_stack.pop();
    }
    
Sdf::SynTree d_root;
	QStack<Sdf::SynTree*> d_stack;
	void addTerminal() {
		Sdf::SynTree* n = new Sdf::SynTree( d_cur ); d_stack.top()->d_children.append(n);
	}



	Parser(Sdf::Lexer *scanner,Sdf::Errors*);
	~Parser();
	void SemErr(const char* msg);

	void Sdf();
	void delay_file();
	void sdf_header();
	void cell();
	void sdf_version();
	void design_name();
	void date();
	void vendor();
	void program_name();
	void program_version();
	void hierarchy_divider();
	void voltage();
	void process();
	void temperature();
	void time_scale();
	void qstring();
	void hchar();
	void rnumber_or_rtriple_();
	void timescale_number();
	void timescale_unit();
	void celltype();
	void cell_instance();
	void timing_spec();
	void hierarchical_identifier();
	void del_spec();
	void tc_spec();
	void lbl_spec();
	void te_spec();
	void deltype();
	void tchk_def();
	void te_def();
	void lbl_type();
	void absolute_deltype();
	void increment_deltype();
	void pathpulse_deltype();
	void pathpulsepercent_deltype();
	void input_output_path();
	void value();
	void del_def();
	void port_instance();
	void iopath_def();
	void cond_def();
	void condelse_def();
	void port_def();
	void interconnect_def();
	void netdelay_def();
	void device_def();
	void port_spec();
	void retain_def();
	void delval_list();
	void retval_list();
	void conditional_port_expr();
	void concat_expression();
	void net_spec();
	void setup_timing_check();
	void hold_timing_check();
	void setuphold_timing_check();
	void recovery_timing_check();
	void removal_timing_check();
	void recrem_timing_check();
	void skew_timing_check();
	void bidirectskew_timing_check();
	void width_timing_check();
	void period_timing_check();
	void nochange_timing_check();
	void port_tchk();
	void rvalue();
	void scond();
	void ccond();
	void timing_check_condition();
	void cns_def();
	void tenv_def();
	void path_constraint();
	void period_constraint();
	void sum_constraint();
	void diff_constraint();
	void skew_constraint();
	void name();
	void exception();
	void constraint_path();
	void arrival_env();
	void departure_env();
	void slack_env();
	void waveform_env();
	void port_edge();
	void real_number();
	void edge_list();
	void pos_or_neg_pair_();
	void signed_real_number();
	void lbl_def();
	void identifier();
	void edge_identifier();
	void integer();
	void port();
	void port_or_net_();
	void delval();
	void conditional_port_expr_nlr_();
	void unary_operator();
	void scalar_constant();
	void binary_operator();
	void scalar_node();
	void equality_operator();
	void inversion_operator();
	void sign();

	void Parse();

}; // end Parser

} // namespace


#endif

