
#include <string.h>
#include <stdio.h>

#include "pbs_job.h"
#include "attribute.h"

job::job() : ji_modified(false)
  {
  memset(&ji_qs, 0, sizeof(ji_qs));
  ji_qs.qs_version = PBS_QS_VERSION;

  init_wattr();
  }



void job::to_json(

  Json::Value &jv)

  {
  }



void job::init_wattr()

  {
  for (unsigned int i = 0; i < JOB_ATR_LAST; i++)
    clear_attr(&this->ji_wattr[i], &job_attr_def[i]);
  }


job::~job()
  {
  for (unsigned int i = 0;i < JOB_ATR_LAST;i++)
    {
    job_attr_def[i].at_free(&ji_wattr[i]);
    }
  }
  
int job::get_state() const

  {
  return(this->ji_qs.ji_state);
  }
  
int job::get_substate() const

  {
  return(this->ji_qs.ji_substate);
  }

const char *job::get_jobid() const

  {
  return(this->ji_qs.ji_jobid);
  }

const char *job::get_destination() const
  {
  return(this->ji_qs.ji_destin);
  }

const char *job::get_queue() const
  {
  return(this->ji_qs.ji_queue);
  }

time_t job::get_start_time() const
  {
  return(this->ji_qs.ji_stime);
  }

int job::get_svrflags() const
  {
  return(this->ji_qs.ji_svrflags);
  }

pbs_net_t job::get_ji_momaddr() const
  {
  return(this->ji_qs.ji_un.ji_exect.ji_momaddr);
  }

unsigned short job::get_ji_momport() const
  {
  return(this->ji_qs.ji_un.ji_exect.ji_momport);
  }

unsigned short job::get_ji_mom_rmport() const
  {
  return(this->ji_qs.ji_un.ji_exect.ji_mom_rmport);
  }

const char *job::get_fileprefix() const
  {
  return(this->ji_qs.ji_fileprefix);
  }

void job::set_fileprefix(

  const char *fileprefix)

  {
  snprintf(this->ji_qs.ji_fileprefix, sizeof(this->ji_qs.ji_fileprefix), "%s", fileprefix);
  }

void job::set_qs_version(

  int version)

  {
  this->ji_qs.qs_version = version;
  }

int job::get_qs_version() const
  {
  return(this->ji_qs.qs_version);
  }

int job::get_un_type() const
  {
  return(this->ji_qs.ji_un_type);
  }

inline bool job::is_valid_attr_index(

  int index,
  int type) const

  {
  return((index >= 0) && (index < JOB_ATR_LAST) && (job_attr_def[index].at_type == type));
  }

inline bool job::is_valid_attr_index_set(

  int index,
  int type) const

  {
  return(is_valid_attr_index(index, type) && this->ji_wattr[index].at_flags & ATR_VFLAG_SET);
  }

bool job::is_attr_set(

  int index) const

  {
  return((index >= 0) && (index < JOB_ATR_LAST) && (this->ji_wattr[index].at_flags & ATR_VFLAG_SET));
  }

pbs_attribute *job::get_attr(

  int index)

  {
  pbs_attribute *pattr = NULL;

  if ((index >= 0) &&
      (index < JOB_ATR_LAST))
    pattr = this->ji_wattr + index;

  return(pattr);
  }
  
struct jobfix &job::get_jobfix()

  {
  return(this->ji_qs);
  }

complete_req *job::get_creq_attr(
    
  int index) const

  {
  complete_req *cr = NULL;

  if (is_valid_attr_index_set(index, ATR_TYPE_REQ))
    cr = (complete_req *)this->ji_wattr[index].at_val.at_ptr;

  return(cr);
  }

struct timeval *job::get_tv_attr(
    
  int index)

  {
  struct timeval *tv = NULL;

  if (is_valid_attr_index_set(index, ATR_TYPE_TV))
    tv = &(this->ji_wattr[index].at_val.at_timeval);
  else if (is_valid_attr_index(index, ATR_TYPE_TV))
    {
    memset(&this->ji_wattr[index].at_val.at_timeval, 0,
      sizeof(this->ji_wattr[index].at_val.at_timeval));
    this->ji_wattr[index].at_flags |= ATR_VFLAG_SET;
    tv = &(this->ji_wattr[index].at_val.at_timeval);
    }

  return(tv);
  }

struct array_strings *job::get_arst_attr(
  
  int index)

  {
  struct array_strings *arst = NULL;

  if (is_valid_attr_index_set(index, ATR_TYPE_ARST))
    arst = this->ji_wattr[index].at_val.at_arst;

  return(arst);
  }

tlist_head job::get_list_attr(

  int index)

  {
  tlist_head th;

  if (is_valid_attr_index_set(index, ATR_TYPE_LIST))
    th = this->ji_wattr[index].at_val.at_list;

  return(th);
  }

char job::get_char_attr(
    
  int index) const

  {
  char c = '\0';

  if (is_valid_attr_index_set(index, ATR_TYPE_CHAR))
    c = this->ji_wattr[index].at_val.at_char;

  return(c);
  }

long job::get_long_attr(
   
  int index) const

  {
  long l = 0;

  if (is_valid_attr_index_set(index, ATR_TYPE_LONG))
    l = this->ji_wattr[index].at_val.at_long;

  return(l);
  }

long long job::get_ll_attr(
    
  int index) const

  {
  long long l = 0;

  if (is_valid_attr_index_set(index, ATR_TYPE_LL))
    l = this->ji_wattr[index].at_val.at_ll;

  return(l);
  }

const char *job::get_str_attr(
    
  int index) const

  {
  const char *str = NULL;

  if (is_valid_attr_index_set(index, ATR_TYPE_STR))
    str = this->ji_wattr[index].at_val.at_str;

  return(str);
  }

bool job::get_bool_attr(
    
  int index) const

  {
  bool b = false;

  if (is_valid_attr_index_set(index, ATR_TYPE_BOOL))
    b = this->ji_wattr[index].at_val.at_bool;

  return(b);
  }

std::vector<resource> *job::get_resc_attr(
    
  int index)

  {
  std::vector<resource> *r = NULL;
  
  if (is_valid_attr_index_set(index, ATR_TYPE_RESC))
    r = (std::vector<resource> *)this->ji_wattr[index].at_val.at_ptr;

  return(r);
  }

void job::set_route_queue_time(

  time_t t)

  {
  this->ji_qs.ji_un.ji_routet.ji_quetime = t;
  }

void job::set_exec_exitstat(

  int ev)

  {
  this->ji_qs.ji_un.ji_exect.ji_exitstat = ev;
  }

void job::set_un_type(

  int type)

  {
  this->ji_qs.ji_un_type = type;
  }

void job::set_destination(

  const char *destination)

  {
  snprintf(this->ji_qs.ji_destin, sizeof(this->ji_qs.ji_destin), "%s", destination);
  }

void job::set_queue(

  const char *queue)

  {
  snprintf(this->ji_qs.ji_queue, sizeof(this->ji_qs.ji_queue), "%s", queue);
  }
  
void job::set_state(
    
  int state)

  {
  this->ji_qs.ji_state = state;
  }

void job::set_substate(
    
  int substate)

  {
  this->ji_qs.ji_substate = substate;
  }

void job::set_jobid(
    
  const char *jobid)

  {
  snprintf(this->ji_qs.ji_jobid, sizeof(this->ji_qs.ji_jobid), "%s", jobid);
  }

void job::set_start_time(
    
  time_t st)

  {
  this->ji_qs.ji_stime = st;
  }

void job::set_svrflags(

  int svrflags)

  {
  this->ji_qs.ji_svrflags = svrflags;
  }

void job::set_ji_momaddr(

  pbs_net_t momaddr)

  {
  this->ji_qs.ji_un.ji_exect.ji_momaddr = momaddr;
  }

void job::set_ji_momport(

  unsigned short momport)

  {
  this->ji_qs.ji_un.ji_exect.ji_momport = momport;
  }

void job::set_ji_mom_rmport(

  unsigned short mom_rmport)

  {
  this->ji_qs.ji_un.ji_exect.ji_mom_rmport = mom_rmport;
  }

void job::set_modified(

  bool m)

  {
  this->ji_modified = m;
  }

int job::set_resc_attr(

  int                    index,
  std::vector<resource> *resources)

  {
  int rc = -1;

  if (is_valid_attr_index(index, ATR_TYPE_RESC))
    {
    if (this->ji_wattr[index].at_flags & ATR_VFLAG_SET)
      {
      std::vector<resource> *old = (std::vector<resource> *)this->ji_wattr[index].at_val.at_ptr;
      if (old != NULL)
        delete old;
      }
    else
      this->ji_wattr[index].at_flags |= ATR_VFLAG_SET;
  
    this->ji_wattr[index].at_flags |= ATR_VFLAG_MODIFY;
    this->ji_wattr[index].at_val.at_ptr = resources;
    rc = PBSE_NONE;
    }
  
  return(rc);
  }

int job::set_creq_attr(
    
  int           index,
  complete_req *cr)

  {
  int rc = -1;

  if (is_valid_attr_index(index, ATR_TYPE_REQ))
    {
    if (this->ji_wattr[index].at_flags & ATR_VFLAG_SET)
      {
      complete_req *old = (complete_req *)this->ji_wattr[index].at_val.at_ptr;
      if (old != NULL)
        delete old;
      }
    else
      this->ji_wattr[index].at_flags |= ATR_VFLAG_SET;

    this->ji_wattr[index].at_flags |= ATR_VFLAG_MODIFY;
      
    this->ji_wattr[index].at_val.at_ptr = cr;
    rc = PBSE_NONE;
    }
  
  return(rc);
  }

int job::set_tv_attr(
    
  int             index,
  struct timeval *tv)

  {
  int rc = -1;

  if (is_valid_attr_index(index, ATR_TYPE_TV))
    {
    rc = PBSE_NONE;

    this->ji_wattr[index].at_flags |= ATR_VFLAG_SET | ATR_VFLAG_MODIFY;
    memcpy(&(this->ji_wattr[index].at_val.at_timeval), tv, sizeof(struct timeval));
    }

  return(rc);
  }

int job::set_arst_attr(
    
  int                   index,
  struct array_strings *arst)

  {
  int rc = -1;

  if (is_valid_attr_index(index, ATR_TYPE_ARST))
    {
    rc = PBSE_NONE;

    free_arst(this->ji_wattr + index);
    this->ji_wattr[index].at_flags |= ATR_VFLAG_SET;
    this->ji_wattr[index].at_val.at_arst = arst;
    }

  return(rc);
  }

int job::set_char_attr(
    
  int  index,
  char c)

  {
  int rc = -1;

  if (is_valid_attr_index(index, ATR_TYPE_CHAR))
    {
    rc = PBSE_NONE;

    this->ji_wattr[index].at_flags |= ATR_VFLAG_SET | ATR_VFLAG_MODIFY;
    this->ji_wattr[index].at_val.at_char = c;
    }

  return(rc);
  } // END set_char_attr()

int job::set_long_attr(
    
  int  index,
  long l)

  {
  int rc = -1;

  if (is_valid_attr_index(index, ATR_TYPE_LONG))
    {
    rc = PBSE_NONE;

    this->ji_wattr[index].at_flags |= ATR_VFLAG_SET | ATR_VFLAG_MODIFY;
    this->ji_wattr[index].at_val.at_long = l;
    }

  return(rc);
  } // END set_long_attr()

int job::set_ll_attr(
    
  int       index,
  long long ll)

  {
  int rc = -1;

  if (is_valid_attr_index(index, ATR_TYPE_LL))
    {
    rc = PBSE_NONE;

    this->ji_wattr[index].at_flags |= ATR_VFLAG_SET | ATR_VFLAG_MODIFY;
    this->ji_wattr[index].at_val.at_ll = ll;
    }

  return(rc);
  } // END set_ll_attr()

int job::set_str_attr(

  int   index,
  char *str)

  {
  int rc = -1;

  if (is_valid_attr_index(index, ATR_TYPE_STR))
    {
    rc = PBSE_NONE;

    free_str(this->ji_wattr + index);
    this->ji_wattr[index].at_flags |= ATR_VFLAG_SET | ATR_VFLAG_MODIFY;
    this->ji_wattr[index].at_val.at_str = str;
    }

  return(rc);
  }
 
int job::set_bool_attr(
    
  int  index,
  bool b)

  {
  int rc = -1;

  if (is_valid_attr_index(index, ATR_TYPE_BOOL))
    {
    rc = PBSE_NONE;

    this->ji_wattr[index].at_flags |= ATR_VFLAG_SET | ATR_VFLAG_MODIFY;
    this->ji_wattr[index].at_val.at_bool = b;
    }

  return(rc);
  }

void job::free_attr(

  int index)

  {
  if ((index >= 0) &&
      (index < JOB_ATR_LAST))
    job_attr_def[index].at_free(this->ji_wattr + index);
  }

void job::unset_attr(

  int index)

  {
  if ((index >= 0) &&
      (index < JOB_ATR_LAST))
    this->ji_wattr[index].at_flags &= ~ATR_VFLAG_SET;
  }

void job::set_attr(

  int index)

  {
  if ((index >= 0) &&
      (index < JOB_ATR_LAST))
    this->ji_wattr[index].at_flags |= ATR_VFLAG_SET;
  }
  
void job::set_fromsock(
    
  int sock)

  {
  this->ji_qs.ji_un.ji_newt.ji_fromsock = sock;
  }

int job::get_fromsock() const
  {
  return(this->ji_qs.ji_un.ji_newt.ji_fromsock);
  }

void job::set_fromaddr(

  pbs_net_t fromaddr)

  {
  this->ji_qs.ji_un.ji_newt.ji_fromaddr = fromaddr;
  }

void job::set_scriptsz(
    
  size_t scriptsz)

  {
  this->ji_qs.ji_un.ji_newt.ji_scriptsz = scriptsz;
  }

size_t job::get_scriptsz() const
  {
  return(this->ji_qs.ji_un.ji_newt.ji_scriptsz);
  }

int job::get_attr_flags(

  int index) const

  {
  int flags = 0;

  if ((index >= 0) &&
      (index < JOB_ATR_LAST))
    flags = this->ji_wattr[index].at_flags;
  
  return(flags);
  }

time_t job::get_route_retry_time() const
  {
  return(this->ji_qs.ji_un.ji_routet.ji_rteretry);
  }

void job::set_route_retry_time(

  time_t rt)

  {
  this->ji_qs.ji_un.ji_routet.ji_rteretry = rt;
  }

time_t job::get_route_queue_time() const

  {
  return(this->ji_qs.ji_un.ji_routet.ji_quetime);
  }

void job::set_exgid(

  unsigned int gid)

  {
  this->ji_qs.ji_un.ji_momt.ji_exgid = gid;
  }

void job::set_exuid(

  unsigned int uid)

  {
  this->ji_qs.ji_un.ji_momt.ji_exuid = uid;
  }

unsigned int job::get_exgid() const
  {
  return(this->ji_qs.ji_un.ji_momt.ji_exgid);
  }

unsigned int job::get_exuid() const
  {
  return(this->ji_qs.ji_un.ji_momt.ji_exuid);
  }

int job::get_exec_exitstat() const
  {
  return(this->ji_qs.ji_un.ji_exect.ji_exitstat);
  }

bool job::has_been_modified() const
  {
  return(this->ji_modified);
  }

void job::set_attr_flag_bm(

  int index,
  int bm)

  {
  if ((index >= 0) &&
      (index < JOB_ATR_LAST))
    this->ji_wattr[index].at_flags |= bm;
  }

void job::set_attr_flag(

  int index,
  int flag_val)

  {
  if ((index >= 0) &&
      (index < JOB_ATR_LAST))
    this->ji_wattr[index].at_flags = flag_val; 
  }
  
pbs_net_t job::get_svraddr() const

  {
  return(this->ji_qs.ji_un.ji_momt.ji_svraddr);
  }

int job::get_mom_exitstat() const
  {
  return(this->ji_qs.ji_un.ji_momt.ji_exitstat);
  }

void job::set_mom_exitstat(

  int ev)

  {
  this->ji_qs.ji_un.ji_momt.ji_exitstat = ev;
  }

void job::set_svraddr(

  pbs_net_t addr)

  {
  this->ji_qs.ji_un.ji_momt.ji_svraddr = addr;
  }

pbs_net_t job::get_fromaddr() const
  {
  return(this->ji_qs.ji_un.ji_newt.ji_fromaddr);
  }
  
int job::get_ordering() const
  {
  return(this->ji_qs.ji_ordering);
  }

int job::get_priority() const
  {
  return(this->ji_qs.ji_priority);
  }

