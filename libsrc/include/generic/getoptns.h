#ifndef __Header_getoptns__
#define __Header_getoptns__

#include "errclass.h"

class GetNamedOptions : public ErrorsInClass {
private:
	const char *		command_string;
	int			argc_start;
	const char *const *	argv_start;
	bool *			argv_used;
	int 			argc_left;
private:
	bool	match(const char *arg,const char *index);

	bool	extract(const char *index,const char * src,long &dst);
	bool	extract(const char *index,const char * src,unsigned long &dst);
	bool	extract(const char *index,const char * src,long long &dst);
	bool	extract(const char *index,const char * src,int &dst);
	bool	extract(const char *index,const char * src,unsigned int &dst);
	bool	extract(const char *index,const char * src,float &dst);
	bool	extract(const char *index,const char * src,double &dst);
	bool	extract(const char *index,const char * src,const char * &dst);

	bool 		get(const char *const *indexes,const char * &value,int& which);

	int		findUnusedOption(void);
	int		findUnusedOption(const char *index);
	int		findUnusedOption(const char *const *indexes,int& which);
	int		findUnusedValue(int n);
	void		firstValue(void);
	int		areMoreValues(void);
	void		bumpValue(void);
	const char *	value(int n);
public:
			GetNamedOptions(int argc,const char *const *argv);
	virtual		 ~GetNamedOptions();

	bool 		get(const char *index);
	bool 		get(const char *const *indexes);
	bool		operator[](const char *index);
	bool		operator[](const char *const *indexes);

	bool 		get(const char *index,int &value);
	bool 		get(const char *index,unsigned int &value);
	bool 		get(const char *index,unsigned long &value);
	bool 		get(const char *index,long &value);
	bool 		get(const char *index,long long &value);
	bool 		get(const char *index,float &value);
	bool 		get(const char *index,double &value);
	bool 		get(const char *index,const char * &value);

	int 		get(const char *index,unsigned int *ptr,int num);
	int 		get(const char *index,int *ptr,int num);
	int 		get(const char *index,unsigned long *ptr,int num);
	int 		get(const char *index,long *ptr,int num);
	int 		get(const char *index,long long *ptr,int num);
	int 		get(const char *index,float *ptr,int num);
	int 		get(const char *index,double *ptr,int num);
	int 		get(const char *index,const char **ptr,int num);

	bool 		get(const char *const *indexes,int &value);
	bool 		get(const char *const *indexes,unsigned int &value);
	bool 		get(const char *const *indexes,unsigned long &value);
	bool 		get(const char *const *indexes,long &value);
	bool 		get(const char *const *indexes,long long &value);
	bool 		get(const char *const *indexes,float &value);
	bool 		get(const char *const *indexes,double &value);
	bool 		get(const char *const *indexes,const char * &value);

	int 		get(const char *const *indexes,unsigned int *ptr,int num);
	int 		get(const char *const *indexes,int *ptr,int num);
	int 		get(const char *const *indexes,unsigned long *ptr,int num);
	int 		get(const char *const *indexes,long *ptr,int num);
	int 		get(const char *const *indexes,long long *ptr,int num);
	int 		get(const char *const *indexes,float *ptr,int num);
	int 		get(const char *const *indexes,double *ptr,int num);
	int 		get(const char *const *indexes,const char **ptr,int num);

	int 		operator!();
	void 		operator++();		// prefix  ++i
	void 		operator++(int);	// postfix i++
	const char *	operator()();
	const char *	operator[](int num);

	const char *	command(void) const;

	void		done(void);
};

#endif /* __Header_getoptns__ */
