#include <iostream>
#include <vector>
extern "C" {
#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>
};

using namespace std;

class PRegex : public string
{
	pcre2_code* re;
	size_t erroroffset;
	int errornumber;
	pcre2_match_data *md;
	int rc;
	const char *sub;

	public:
		PRegex (string r)
			: string(r)
		{
			re=pcre2_compile ((PCRE2_SPTR)string::c_str(),
					PCRE2_ZERO_TERMINATED,
					0, &errornumber, &erroroffset,
					(pcre2_compile_context_8*)NULL);
			if (re==NULL)
			{
				throw errornumber;
			}
			md=pcre2_match_data_create_from_pattern (re, NULL);
			if (!md)
				throw 2;
		}
		~PRegex ()
		{
			pcre2_match_data_free (md);
			pcre2_code_free (re);
		}
		
		bool operator==(string s)
		{
			sub=s.c_str ();
			bool res(
					(rc=pcre2_match (
						re,
						(PCRE2_SPTR) s.c_str (),
						s.length (),
						0, 0, md, NULL
						))>0
				);
			return res;
		}
		string operator[](int n)
		{
			PCRE2_SIZE *ov=pcre2_get_ovector_pointer (md);
			if (n>=rc) throw 1;
			return string(sub+ov[2*n], (ov[2*n+1]-ov[2*n]));
		};
};


class name : public string {
	public:
	name (char *str) : string (str) {}
	name (string str) : string (str) {}
	auto match (string n)
		-> bool
	{
		PRegex r(n);
		return r==*this;
	}
	auto operator== (name n)
		-> bool
	{
		PRegex r(n);
		return r==*this;
	}
	auto operator== (PRegex& r)
		-> bool
	{
		bool res(r==*this);
		return res;
	}
};


int main (int argsc, char **args)
{
	for (char **p=args; p && *p; p++)
	{
		PRegex prx("(^|/)([A-Za-z0-9]+)[_-]+((\\.?\\d{1,2})+)$");

		if (name(*p)==prx)
		{
			cout << prx[2] << " " << prx[3] << endl;
		}
	}
	return( 0 );
}
