#include "license_pbs.h" /* See here for the software license */
/*
 * qmgr
 *      The qmgr command provides an administrator interface to the batch
 *      system.  The command reads directives from standard input.  The syntax
 *      of each directive is checked and the appropriate request is sent to the
 *      batch server or servers.
 *
 * Synopsis:
 *      qmgr [-a] [-c command] [-e] [-n] [-z] [server...]
 *
 * Options:
 *      -a      Abort qmgr on any syntax errors or any requests rejected by a
 *              server.
 *
 *      -c command
 *              Execute a single command and exit qmgr.
 *
 *      -e      Echo all commands to standard output.
 *
 *      -n      No commands are executed, syntax checking only is performed.
 *
 *      -z      No errors are written to standard error.
 *
 * Arguments:
 *      server...
 *              A list of servers to administer.  If no servers are given, then
 *              use the default server.
 *
 *
 * Exitcodes:
 *   0 - successful
 *   1 - error in parse
 *   2 - error in execute
 *   3 - error connect_servers
 *   4 - error set_active
 *   5 - memory allocation error
 *
 *
 *
 *
 * Written by:
 *      Bruce Kelly
 *      National Energy Research Supercomputer Center
 *      Livermore, CA
 *      March, 1993
 *
 */

#include <pbs_config.h>

#include <stdio.h>
#include <ctype.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef HAVE_READLINE
/* some versions have a buggy readline header */
#undef HAVE_CONFIG_H
#include <readline/tilde.h>
#define HAVE_CONFIG_H

#include <readline/readline.h>
#include <readline/history.h>
#endif /* END HAVE_READLINE */

/* PBS include files */
#include "cmds.h"
#include "qmgr.h"
#include "net_cache.h"



/* Global Variables */

struct server *servers = NULL;  /* Linked list of server structures */
int nservers = 0;               /* Number of servers */
int zopt = FALSE;  /* -z option */

/* active objects */

struct objname *active_servers;

struct objname *active_queues;

struct objname *active_nodes;

int main(

  int    argc,
  char **argv)

  {
  static char opts[] = "ac:enz"; /* See man getopt */
  static char usage[] = "Usage: qmgr [-a] [-c command] [-e] [-n] [-z] [server...]\n";
  int aopt = FALSE;  /* -a option */
  int eopt = FALSE;  /* -e option */
  int nopt = FALSE;  /* -n option */
  char *copt = NULL;  /* -c command option */
  int c;   /* Individual option */
  int errflg = 0;  /* Error flag */
  char request[MAX_LINE_LEN]; /* Current request */
  int oper;   /* Operation: create, delete, set, unset, list, print */
  int type;   /* Object type: server or queue */
  char *name = NULL;  /* Object name */

  struct attropl *attribs = NULL;   /* Pointer to attribute list */

  struct objname *svrs;
  
  initialize_network_info();

  /* Command line options */

  while ((c = getopt(argc, argv, opts)) != EOF)
    {
    switch (c)
      {

      case 'a':

        aopt = TRUE;

        break;

      case 'c':

        copt = optarg;

        break;

      case 'e':

        eopt = TRUE;

        break;

      case 'n':

        nopt = TRUE;

        break;

      case 'z':

        zopt = TRUE;

        break;

      case '?':

      default:

        errflg++;

        break;
      }  /* END switch(c) */
    }    /* END while ((c = getopt(argc,argv,opts)) != EOF) */

  if (errflg)
    {
    if (! zopt) fprintf(stderr,"%s", usage);

    exit(1);
    }

  if (argc > optind)
    svrs = strings2objname(&argv[optind], argc - optind, MGR_OBJ_SERVER);
  else
    svrs = default_server_name();

  /*
   * If no command was given on the command line, then read them from
   * stdin until end-of-file.  Otherwise, execute the one command only.
   */

  if (copt == NULL)
    {
    printf("Max open servers: %d\n",
           pbs_query_max_connections());

#ifdef HAVE_READLINE
    rl_bind_key('\t', rl_insert);
#endif /* HAVE_READLINE */

    while (get_request(request) != EOF)
      {
      if (eopt)
        printf("%s\n", request);

      errflg = parse(request, &oper, &type, &name, &attribs);

      if (errflg == -1)  /* help */
        continue;

      if (aopt && errflg)
        clean_up_and_exit(1);

      if (!nopt && !errflg)
        {
        errflg = connect_servers(svrs, ALL_SERVERS);

        if ((aopt && errflg) || (nservers == 0))
          clean_up_and_exit(3);

        errflg = set_active(MGR_OBJ_SERVER, svrs);

        if (errflg && aopt)
          clean_up_and_exit(4);

        errflg = execute(
                   aopt,
                   oper,
                   type,
                   name,
                   attribs);

        if (aopt && errflg)
          clean_up_and_exit(2);
        else
          {
          clean_up();
          servers = NULL;
          active_servers = NULL;
          }
        }
      }
    }
  else
    {
    errflg = connect_servers(svrs, ALL_SERVERS);

    if ((aopt && errflg) || (nservers == 0))
      clean_up_and_exit(3);

    errflg = set_active(MGR_OBJ_SERVER, svrs);

    if (errflg && aopt)
      clean_up_and_exit(4);

    if (eopt)
      printf("%s\n", copt);

    errflg = parse(copt, &oper, &type, &name, &attribs);

    if (aopt && errflg)
      clean_up_and_exit(1);

    if (!nopt && !errflg)
      {
      errflg = execute(
                 aopt,
                 oper,
                 type,
                 name,
                 attribs);

      if (aopt && errflg)
        clean_up_and_exit(2);
      }
    }

  clean_up_and_exit(0);

  return(0);
  }  /* END main() */





/*
 *
 * attributes - parse attribute-value pairs in the format:
 *       attribute OP value
 *       which are on the qmgr input
 *
 *    attrs        Text of the attribute-value pairs.
 *    OUT: attrlist     Address of the attribute-value structure.
 *    doper directive operation type (create, delete, set, ...)
 *
 *  Returns:
 *        This routine returns zero upon successful completion.  If there is
 *        a syntax error, it will return the index into attrs where the error
 *        occurred.
 *
 *  Note:
 *        The following is an example of the text input and what the resulting
 *        data structure should look like.
 *
 *      a1 = v1, a2.r2 += v2 , a3-=v3
 *
 *      attrlist ---> struct attropl *
 *                      |
 *                      |
 *                      \/
 *                      "a1"
 *                      ""
 *                      "v1"
 *                      SET
 *                      ---------
 *                              |
 *                      "a2"   <-
 *                      "r2"
 *                      "v2"
 *                      INCR
 *                      ---------
 *                              |
 *                      "a3"   <-
 *                      ""
 *                      "v3"
 *                      DECR
 *                      NULL
 */

int attributes(

  char            *attrs,
  struct attropl **attrlist,
  int              doper)

  {
  char *c;            /* Pointer into the attrs text */
  char *start;        /* Pointer to the start of a word */
  char q;             /* Quote character */
  int ltxt;           /* Length of a word */

  struct attropl *paol;

  /* Free the space from the previous structure */
  freeattropl(*attrlist);
  *attrlist = NULL;

  /* Is there any thing to parse? */

  c = attrs;

  while (White(*c))
    c++;

  if (EOL(*c))
    {
    return(0);
    }

  /* Parse the attribute-values */

  while (TRUE)
    {
    /* Get the attribute and resource */

    while (White(*c))
      c++;

    if (!EOL(*c))
      {
      start = c;

      while ((*c != '.') && (*c != ',') && !EOL(*c) && !Oper(c) && !White(*c))
        c++;

      if (c == start)
        {
        /* No attribute */

        if (start == attrs)
          start++;

        return(start - attrs);
        }

      /* Allocate storage for attribute structure */

      Mstruct(paol, struct attropl);

      paol->name = NULL;

      paol->resource = NULL;

      paol->value = NULL;

      paol->next = *attrlist;

      *attrlist = paol;

      /* Copy attribute into structure */

      ltxt = c - start;

      Mstring(paol->name, ltxt + 1);

      strncpy(paol->name, start, ltxt);

      paol->name[ltxt] = '\0';

      /* Resource, if any */

      if (*c == '.')
        {
        start = ++c;

        while (!White(*c) && !Oper(c) && !EOL(*c))
          c++;

        ltxt = c - start;

        if (ltxt == 0)   /* No resource */
          {
          return(start - attrs);
          }

        Mstring(paol->resource, ltxt + 1);

        strncpy(paol->resource, start, ltxt);

        paol->resource[ltxt] = '\0';
        }
      }
    else
      {
      return(c - attrs);
      }

    /* Get the operator */

    while (White(*c))
      c++;

    if (!EOL(*c))
      {
      switch (*c)
        {

        case '=':

          paol->op = SET;
          c++;

          break;

        case '+':

          paol->op = INCR_OLD;
          c += 2;

          break;

        case '-':

          paol->op = DECR;
          c += 2;

          break;

        case ',':

          /* Attribute with no value */

          Mstring(paol->value, 1);
          paol->value[0] = '\0';

          goto next;

          /*NOTREACHED */

          break;

        default:

          return(c - attrs);

          /*NOTREACHED*/

          break;
        }

      /* The unset command must not have a operator or value */

      if (doper == MGR_CMD_UNSET)
        {
        return(c - attrs);
        }
      }
    else if ((doper != MGR_CMD_CREATE) && (doper != MGR_CMD_SET))
      {
      Mstring(paol->value, 1);

      paol->value[0] = '\0';

      return(0);
      }
    else
      {
      return(c - attrs);
      }

    /* Get the value */

    while (White(*c))
      c++;

    if (!EOL(*c))
      {
      start = c;

      if ((*c == '"') || (*c == '\''))
        {
        q = *c;

        start = ++c;

        while ((*c != q) && !EOL(*c))
          c++;

        if (EOL(*c))
          {
          return(start - attrs);
          }

        ltxt = c - start;

        Mstring(paol->value, ltxt + 1);

        if (ltxt > 0)
          strncpy(paol->value, start, ltxt);

        paol->value[ltxt] = '\0';

        c++;
        }
      else
        {
        while (!White(*c) && (*c != ',') && !EOL(*c))
          c++;

        ltxt = c - start;

        Mstring(paol->value, ltxt + 1);

        strncpy(paol->value, start, ltxt);

        paol->value[ltxt] = '\0';
        }
      }
    else
      {
      return(c - attrs);
      }

    /* See if there is another attribute-value pair */

next:

    while (White(*c))
      c++;

    if (EOL(*c))
      {
      return(0);
      }

    if (*c == ',')
      {
      c++;
      }
    else
      {
      return(c - attrs);
      }
    }    /* END while (TRUE) */

  /* NOTE:  original code did not explicitly return, what should be returned at this point? */

  return(0);
  }  /* END attributes() */





/*
 *
 * attropl2attrl - convert an attropl struct to an attrl struct
 *
 *   from - the attropl struct to convert
 *
 * returns newly converted attrl struct
 *
 */

struct attrl *attropl2attrl(

        struct attropl *from)

  {

  struct attrl *ap = NULL, *rattrl = NULL;

  while (from != NULL)
    {
    if (ap == NULL)
      {
      Mstruct(ap, struct attrl);
      rattrl = ap;
      }
    else
      {
      Mstruct(ap->next, struct attrl);
      ap = ap->next;
      }

    ap->name = NULL;

    ap->resource = NULL;
    ap->value = NULL;
    ap->next = NULL;

    if (from->name != NULL)
      {
      Mstring(ap->name, strlen(from->name) + 1);
      strcpy(ap->name, from->name);
      }

    if (from->resource != NULL)
      {
      Mstring(ap->resource, strlen(from->resource) + 1);
      strcpy(ap->resource, from->resource);
      }

    if (from->value != NULL)
      {
      Mstring(ap->value, strlen(from->value) + 1);
      strcpy(ap->value, from->value);
      }

    from = from->next;
    }  /* END while (from != NULL) */

  return(rattrl);
  }  /* END attropl2attrl() */





/*
 *
 * make_connection - open a connection to the server and assign
 *     server entry
 *
 *   name - name of server to connect to
 *
 * returns server struct if connection can be made or NULL if not
 *
 */

struct server *make_connection(

        char *name)

  {
  int connection;

  struct server *svr = NULL;

  if ((connection = cnt2server(name)) > 0)
    {
    svr = new_server();

    Mstring(svr->s_name, strlen(name) + 1);

    strcpy(svr->s_name, name);

    svr->s_connect = connection;
    }
  else
    {
    if (! zopt)
      fprintf(stderr, "qmgr: cannot connect to server %s (errno=%d) %s\n",
              name, connection * -1, pbs_strerror(connection * -1));
    }

  return(svr);
  }





/*
 *
 * connect_servers - call connect to connect to each server in list
 *     and add then to the global server list
 *
 *   server_names - list of objnames
 *   numservers   - the number of servers to connect to or -1 for all
 *    the servers on the list
 *
 * returns: error if TRUE / success if FALSE
 */

int connect_servers(

  struct objname *server_names,
  int             numservers)

  {
  int error = FALSE;

  struct server *cur_svr;

  struct objname *cur_obj;
  int i;
  int max_servers;

  max_servers = pbs_query_max_connections();

  close_non_ref_servers();

  if (nservers < max_servers)
    {
    cur_obj = server_names;

    /* if numservers == -1 (all servers) the var i will never equal zero */

    for (i = numservers; i != 0 && cur_obj != NULL; i--, cur_obj = cur_obj -> next)
      {
      nservers++;

      if ((cur_svr = make_connection(cur_obj -> svr_name)) == NULL)
        {
        nservers--;
        error = TRUE;
        }

      if (cur_svr != NULL)
        {
        cur_obj -> svr = cur_svr;
        cur_svr -> ref++;
        cur_svr -> next = servers;
        servers = cur_svr;
        }
      }
    }
  else
    {
    if (! zopt) fprintf(stderr, "qmgr: max server connections reached.\n");

    error = 1;
    }

  return error;
  }





/*
 *  blanks - print requested spaces
 *
 *    number - The number spaces
 *
 *  Returns:
 *  Nothing
 */

void blanks(

  int number)

  {
  char spaces[1024];
  int i;

  if (number < 1023)
    {
    for (i = 0;i < number;i++)
      spaces[i] = ' ';

    spaces[i] = '\0';

    if (! zopt) fprintf(stderr, "%s", spaces);
    }
  else
    {
    if (! zopt) fprintf(stderr, "Too many blanks requested.\n");
    }

  return;
  }




/*
 *
 * check_list - check a comma delimited list for valid syntax
 *
 *      list  A comma delimited list.
 *
 * valid syntax: name[@server][,name]
 *  example: batch@svr1,debug
 *
 *  Returns:
 *    0 If the syntax of the list is correct for all commands.
 *        >0 The number of chars into the list where the error occured
 */

int check_list(

  char *list)

  {
  char *foreptr, *backptr;

  backptr = list;

  while (!EOL(*backptr))
    {
    foreptr = backptr;

    /* object names have to start with an alpha character or can be left off
     * if all objects of the same type are wanted
     */

    if (!isalpha(*backptr) && (*backptr != '@') && (*backptr != ':'))
      {
      return(backptr - list ? backptr - list : 1);
      }

    while ((*foreptr != ',') && (*foreptr != '@') && !EOL(*foreptr))
      foreptr++;

    if (*foreptr == '@')
      {
      foreptr++;

      /* error on "name@" or "name@," */

      if (EOL(*foreptr) || *foreptr == ',')
        {
        return(foreptr - list);
        }

      while (!EOL(*foreptr) && *foreptr != ',')
        foreptr++;

      /* error on name@svr@blah */

      if (*foreptr == '@')
        {
        return(foreptr - list);
        }
      }

    if (*foreptr == ',')
      {
      foreptr++;

      /* error on "name," */

      if (EOL(*foreptr))
        {
        return(foreptr - list ? foreptr - list : 1);
        }
      }

    backptr = foreptr;
    }

  /* SUCCESS */

  return(0);
  }  /* END check_list() */





/*
 *
 * disconnect_from_server  - disconnect from one server and clean up
 *
 *   svr - the server to disconnect from
 *
 * returns nothing
 *
 */
void
disconnect_from_server(struct server *svr)
  {
  pbs_disconnect(svr -> s_connect);
  free_server(svr);
  nservers--;
  }



void clean_up()
  {
  struct server *cur_svr, *next_svr;

  cur_svr = servers;

  while (cur_svr)
    {
    next_svr = cur_svr -> next;
    pbs_disconnect(cur_svr->s_connect);
    cur_svr = next_svr;
    nservers--;
    }

  fflush(NULL);   /* peter h IPSec+jan n NANCO 2009 * fix truncated output. */
  }



/*
 *
 * clean_up_and_exit - disconnect from the servers and free memory used
 *       by active object lists, flush output, and then exits
 *
 *   exit_val - value to pass to exit
 *
 * Returns: Never
 *
 */
void clean_up_and_exit(int exit_val)
  {
  clean_up();
  free_objname_list(active_servers);
  free_objname_list(active_queues);
  free_objname_list(active_nodes);
  exit(exit_val);
  }



/*
 *  display - print out all the information in a batch_status struct
 *    in either readable or qmgr input format
 *
 *    otype      Object type
 *    oname      Object name
 *    status     Attribute list of the object
 *    format     True, if the output should be formatted to look like input
 *
 *  Returns:
 *   nothing
 */

void display(

  int                  otype,
  char                *oname,
  struct batch_status *status,
  int                  format)

  {

  struct attrl *attr;
  char *c, *e;
  int l, comma, first, indent_len;

  if (format)
    {
    if (otype == MGR_OBJ_SERVER)
      printf("#\n# Set server attributes.\n#\n");
    else if (otype == MGR_OBJ_QUEUE)
      printf("#\n# Create queues and set their attributes.\n#\n");
    else if (otype == MGR_OBJ_NODE)
      printf("#\n# Create nodes and set their properties.\n#\n");
    }

  while (status != NULL)
    {
    if (otype == MGR_OBJ_SERVER)
      {
      if (!format)
        printf("Server %s\n",
               status->name);
      }
    else if (otype == MGR_OBJ_QUEUE)
      {
      if (format)
        {
        printf("#\n# Create and define queue %s\n#\n",
               status->name);

        printf("create queue %s\n",
               status->name);
        }
      else
        {
        printf("Queue %s\n",
               status->name);
        }
      }
    else if (otype == MGR_OBJ_NODE)
      {
      if (format)
        {
        printf("#\n# Create and define node %s\n#\n", status->name);
        printf("create node %s\n", status->name);
        }
      else
        printf("Node %s\n", status->name);
      }

    attr = status->attribs;

    while (attr != NULL)
      {
      if (format)
        {
        if (is_attr(otype, attr->name, TYPE_ATTR_PUBLIC))
          {
          comma = TRUE;
          first = TRUE;
          c = attr->value;
          e = c;

          while (comma)
            {
            printf("set ");

            if (otype == MGR_OBJ_SERVER)
              printf("server ");

            if (otype == MGR_OBJ_QUEUE)
              printf("queue %s ", status->name);

            if (otype == MGR_OBJ_NODE)
              printf("node %s ", status->name);

            if (attr->name != NULL)
              printf("%s", attr->name);

            if (attr->resource != NULL)
              printf(".%s", attr->resource);

            if (attr->value != NULL)
              {
              if (first)
                printf(" = ");
              else
                printf(" += ");

              first = FALSE;

              while (*e != ',' && *e != '\0')
                e++;

              comma = (*e == ',');

              *e = '\0';

              printf("%s", c);

              e++;

              c = e;
              }
            else
              comma = FALSE;

            printf("\n");
            }
          }
        }
      else
        {
        if (attr->name != NULL)
          printf("\t%s", attr->name);

        if (attr->resource != NULL)
          printf(".%s", attr->resource);

        if ((attr->value != NULL) && (attr->name != NULL) && !strcmp(attr->name, ATTR_mtime))
          {
          time_t epoch;

          epoch = (time_t)atoi(attr->value);
          printf(" = %s", ctime(&epoch));
          }

        else if (attr->value != NULL)
          {
          l = strlen(attr->name) + 8;

          if (attr->resource != NULL)
            l += strlen(attr->resource) + 1;

          l += 3;

          indent_len = l;

          printf(" = ");

          c = attr->value;

          e = c;

          comma = TRUE;

          first = TRUE;

          while (comma)
            {
            while (*e != ',' && *e != '\0')
              e++;

            comma = (*e == ',');

            *e = '\0';

            l += strlen(c) + 1;

            if (!first && (l >= 80))
              {
              printf("\n%*s", indent_len, " ");
              l = strlen(c) + indent_len;

              while (White(*c))
                c++;
              }

            printf("%s", c);

            first = FALSE;

            if (comma)
              {
              printf(",");
              *e = ',';
              }

            e++;

            c = e;
            }

          printf("\n");
          }
        }

      attr = attr->next;
      }

    if (!format)
      printf("\n");

    status = status->next;
    }
  }

/*
 *
 * set_active - sets active objects
 *
 *   obj_type - the type of object - should be caller allocated space
 *   obj_names - names of objects to set active
 *
 * returns 0 on success and non-zero on failure
 *
 */
int
set_active(int obj_type, struct objname *obj_names)
  {

  struct objname *cur_obj = NULL;

  struct server *svr;
  int error = 0;

  if (obj_names != NULL)
    {
    switch (obj_type)
      {

      case MGR_OBJ_SERVER:
        cur_obj = obj_names;

        while (cur_obj != NULL && !error)
          {
          if (cur_obj -> svr == NULL)
            {
            svr = find_server(cur_obj -> obj_name);

            if (svr == NULL)
              error = connect_servers(cur_obj, 1);
            else
              {
              cur_obj -> svr = svr;
              svr -> ref++;
              }
            }

          cur_obj = cur_obj -> next;
          }

        if (!error)
          {
          free_objname_list(active_servers);
          active_servers = obj_names;
          }
        else
          free_objname_list(obj_names);

        break;

      case MGR_OBJ_QUEUE:
        cur_obj = obj_names;

        while (cur_obj != NULL && !error)
          {
          if (cur_obj -> svr_name != NULL)
            {
            if (cur_obj -> svr == NULL)
              if (connect_servers(cur_obj, 1) == TRUE)
                error = 1;
            }

          if (!is_valid_object(cur_obj, MGR_OBJ_QUEUE))
            {
            if (! zopt) fprintf(stderr, "Queue does not exist: %s.\n", cur_obj -> obj_name);

            error = 1;
            }

          cur_obj = cur_obj -> next;
          }

        if (!error)
          {
          free_objname_list(active_queues);
          active_queues = obj_names;
          }

        break;

      case MGR_OBJ_NODE:
        cur_obj = obj_names;

        while (cur_obj != NULL && !error)
          {
          if (cur_obj -> svr_name != NULL)
            {
            if (cur_obj -> svr == NULL)
              if (connect_servers(cur_obj, 1) == TRUE)
                error = 1;
            }

          if (!is_valid_object(cur_obj, MGR_OBJ_NODE))
            {
            if (! zopt) fprintf(stderr, "Node does not exist: %s.\n", cur_obj -> obj_name);

            error = 1;
            }

          cur_obj = cur_obj -> next;
          }

        if (!error)
          {
          free_objname_list(active_nodes);
          active_nodes = obj_names;
          }

        break;

      default:
        error = 1;
      }
    }
  else
    {
    switch (obj_type)
      {

      case MGR_OBJ_SERVER:
        printf("Active servers:\n");
        cur_obj = active_servers;
        break;

      case MGR_OBJ_QUEUE:
        printf("Active queues:\n");
        cur_obj = active_queues;
        break;

      case MGR_OBJ_NODE:
        printf("Active nodes:\n");
        cur_obj = active_nodes;
        break;
      }

    while (cur_obj != NULL)
      {
      if (obj_type == MGR_OBJ_SERVER)
        printf("%s\n", Svrname(cur_obj -> svr));
      else
        printf("%s@%s\n", cur_obj -> obj_name, Svrname(cur_obj -> svr));

      cur_obj = cur_obj -> next;
      }
    }

  return error;
  }





/*
 *  execute - contact the server and execute the command
 *
 *    aopt         True, if the -a option was given.
 *    oper         The command, either create, delete, set, unset or list.
 *   type         The object type, either server or queue.
 *    names        The object name list.
 *    attribs      The attribute list with operators.
 *
 *  Returns:
 *        0 for success non-zero for error
 *
 * Library:
 *      libpbs
 *          pbs_manager
 *          pbs_statserver
 *          pbs_statque
 *          pbs_statfree
 *          pbs_geterrmsg
 */

int execute(

  int             aopt,
  int             oper,     /* I */
  int             type,
  char           *names,
  struct attropl *attribs)

  {
  int len;   /* Used for length of an err msg*/
  int error;   /* Error value returned */
  int perr;   /* Value returned from pbs_manager */
  int local_errno;
  char *errmsg;   /* Error message from pbs_errmsg */
  char errnomsg[256];  /* Error message with pbs_errno */

  struct objname *name;  /* Pointer to a list of object names */

  struct objname *pname = NULL; /* Pointer to current object name */

  struct objname *sname = NULL; /* Pointer to current server name */

  struct objname *svrs;  /* servers to loop through */

  struct attrl *sa;  /* Argument needed for status routines */
  /* Argument used to request queue names */

  struct server *sp;  /* Pointer to server structure */
  /* Return structure from a list or print request */

  struct batch_status *ss = NULL;

  error = 0;

  name = commalist2objname(names, type);

  if (oper == MGR_CMD_ACTIVE)
    {
    return(set_active(type, name));
    }

  if (name == NULL)
    {
    switch (type)
      {
        /* There will always be an active server */

      case MGR_OBJ_SERVER:

        pname = active_servers;
        break;

      case MGR_OBJ_QUEUE:

        if (active_queues != NULL)
          pname = active_queues;
        else
          if (! zopt) fprintf(stderr, "No Active Queues, nothing done.\n");

        break;

      case MGR_OBJ_NODE:

        if (active_nodes != NULL)
          pname = active_nodes;
        else
          if (! zopt) fprintf(stderr, "No Active Nodes, nothing done.\n");

        break;

        /* DIAGTODO: handle new diag type */
      }
    }
  else
    {
    pname = name;
    }

  for (;pname != NULL;pname = pname->next)
    {
    if (pname -> svr_name != NULL)
      svrs = temp_objname(NULL, pname->svr_name, pname->svr);
    else
      svrs = active_servers;

    for (sname = svrs;sname != NULL;sname = sname->next)
      {
      if (sname->svr == NULL)
        {
        error = connect_servers(sname, 1);

        if (error)
          continue;
        }

      sp = sname -> svr;

      if ((oper == MGR_CMD_LIST))
        {
        sa = attropl2attrl(attribs);

        switch (type)
          {

          case MGR_OBJ_SERVER:
            ss = pbs_statserver_err(sp -> s_connect, sa, NULL, &local_errno);
            break;

          case MGR_OBJ_QUEUE:
            ss = pbs_statque_err(sp -> s_connect, pname -> obj_name, sa, NULL, &local_errno);
            break;

          case MGR_OBJ_NODE:
            ss = pbs_statnode_err(sp -> s_connect, pname -> obj_name, sa, NULL, &local_errno);
            break;
            /* DIAGTODO: handle new diag type for list */
            /* DIAGTODO: create a pbs_statdiag() */
          }

        freeattrl(sa);

        perr = (ss == NULL);

        if (! perr)
          display(type, pname -> obj_name, ss, FALSE);

        pbs_statfree(ss);
        }
      else if ((oper == MGR_CMD_PRINT))
        {
        sa = attropl2attrl(attribs);

        switch (type)
          {

          case MGR_OBJ_SERVER:

            if (sa == NULL)
              {
              ss = pbs_statque_err(sp -> s_connect, NULL, NULL, NULL, &local_errno);

              if (ss != NULL)
                display(MGR_OBJ_QUEUE, NULL, ss, TRUE);
              }

            ss = pbs_statserver_err(sp -> s_connect, sa, NULL, &local_errno);

            break;

          case MGR_OBJ_QUEUE:

            ss = pbs_statque_err(sp->s_connect, pname->obj_name, sa, NULL, &local_errno);

            break;

          case MGR_OBJ_NODE:

            ss = pbs_statnode_err(sp->s_connect, pname->obj_name, sa, NULL, &local_errno);

            break;
            /* DIAGTODO: handle new diag type for print */
          }

        freeattrl(sa);

        perr = (ss == NULL);

        if (! perr)
          display(type, pname -> obj_name, ss, TRUE);

        pbs_statfree(ss);
        }
      else
        {
        perr = pbs_manager_err(
                 sp->s_connect,
                 oper,
                 type,
                 pname->obj_name,
                 attribs,
                 NULL,
                 &local_errno);
        }

      if (perr)
        {
        errmsg = pbs_geterrmsg(sp -> s_connect);

        if (errmsg != NULL)
          {
          len = strlen(errmsg) + strlen(pname -> obj_name) + strlen(Svrname(sp)) + 20;

          if (len < 256)
            {
            sprintf(errnomsg, "qmgr obj=%s svr=%s: %s\n",
              pname -> obj_name, Svrname(sp),  errmsg);

            if (! zopt) fprintf(stderr, "%s", errnomsg);
            }
          else
            {
            /*obviously, this is to cover a highly unlikely case*/

            pstderr_big(pname -> obj_name, Svrname(sp), errmsg);
            }
          }
        else
          if (! zopt)
            fprintf(stderr, "qmgr: Error from server\n");

        if (aopt)
          return perr;

        error = perr;
        }

      temp_objname(NULL, NULL, NULL);  /* clears reference count */
      }
    }

  if (name != NULL)
    free_objname_list(name);

  return(error);
  }  /* END execute() */



/*
 * void freeattrl(struct attrl *attr)
 *
 * attr     Pointer to the linked list of attrls to clean up.
 *
 * Returns:
 *  None
 */

void
freeattrl(struct attrl *attr)

  {

  struct attrl *ap;

  while (attr != NULL)
    {
    if (attr->name != NULL) free(attr->name);

    if (attr->resource != NULL) free(attr->resource);

    if (attr->value != NULL) free(attr->value);

    ap = attr->next;

    free(attr);

    attr = ap;
    }

  return;
  }



/*
 * void freeattropl(struct attropl *attr)
 *
 * attr     Pointer to the linked list of attropls to clean up.
 *
 * Returns:
 *  None
 */
void
freeattropl(struct attropl *attr)
  {

  struct attropl *ap;

  while (attr != NULL)
    {
    if (attr->name != NULL) free(attr->name);

    if (attr->resource != NULL) free(attr->resource);

    if (attr->value != NULL) free(attr->value);

    ap = attr->next;

    free(attr);

    attr = ap;
    }
  }



/*
 *
 * commalist2objname - convert a comma separated list of strings to a
 *       linked list of objname structs
 *
 *   names - comma separated list of strings
 *   type - the type of the objects
 *
 * returns linked list of objname structs
 *
 */

struct objname *
      commalist2objname(char *names, int type)
  {
  char *foreptr, *backptr;  /* front and back of words */

  struct objname *objs = NULL;  /* the front of the name object list */

  struct objname *cur_obj;  /* the current name object */

  struct objname *prev_obj = NULL; /* the previous name object */
  int len;    /* length of segment of string */
  char error = 0;   /* error flag */

  if (names != NULL)
    {
    foreptr = backptr = names;

    while (!EOL(*foreptr) && !error)
      {
      while (White(*foreptr)) foreptr++;

      backptr = foreptr;

      while (*foreptr != ',' &&  *foreptr != '@' && !EOL(*foreptr)) foreptr++;

      cur_obj = new_objname();

      cur_obj -> obj_type = type;

      if (*foreptr == '@')
        {
        len = foreptr - backptr;
        Mstring(cur_obj -> obj_name, len + 1);
        strncpy(cur_obj -> obj_name, backptr, len);
        cur_obj -> obj_name[len] = '\0';
        foreptr++;
        backptr = foreptr;

        while (*foreptr != ',' && !EOL(*foreptr)) foreptr++;

        len = foreptr - backptr;

        if (strncmp(backptr, DEFAULT_SERVER, len) == 0)
          {
          Mstring(cur_obj -> svr_name, 1);
          cur_obj -> svr_name[0] = '\0';
          }
        else if (strncmp(backptr, ACTIVE_SERVER, len) == 0)
          cur_obj -> svr_name = NULL;
        else
          {
          Mstring(cur_obj -> svr_name, len + 1);
          strncpy(cur_obj -> svr_name, backptr, len);
          cur_obj -> svr_name[len] = '\0';
          }

        if (!EOL(*foreptr))
          foreptr++;
        }
      else
        {
        len = foreptr - backptr;

        if (type == MGR_OBJ_SERVER && !strcmp(backptr, DEFAULT_SERVER))
          {
          Mstring(cur_obj -> obj_name, 1);
          cur_obj -> obj_name[0] = '\0';
          }
        else
          {
          Mstring(cur_obj -> obj_name, len + 1);
          strncpy(cur_obj -> obj_name, backptr, len);
          cur_obj -> obj_name[len] = '\0';
          }

        if (type == MGR_OBJ_SERVER)
          cur_obj -> svr_name = cur_obj -> obj_name;

        if (!EOL(*foreptr))
          foreptr++;
        }

      if ((cur_obj -> svr = find_server(cur_obj -> svr_name)) != NULL)
        cur_obj -> svr -> ref++;

      if (objs == NULL)
        objs = cur_obj;

      if (prev_obj == NULL)
        prev_obj = cur_obj;
      else if (cur_obj != NULL)
        {
        prev_obj -> next = cur_obj;
        prev_obj = cur_obj;
        }
      }
    }

  if (error)
    {
    free_objname_list(objs);
    return NULL;
    }

  return objs;
  }




/*
 *  get_request - get a qmgr request from the standard input
 *
 *    OUT: request      The buffer for the qmgr request
 *
 *  Returns:
 *        This routine returns zero if there is a command to execute.
 *    Otherwise, it returns EOF.
 *
 * NOTE:
 *      This routine has a static buffer it keeps lines of input in.
 * Since commands can be separated by semicolons, a line may contain
 * more than one command.  In this case, the command is copied to
 * request and the rest of the line is moved up to overwrite the previous
 * command.  Another line is retrieved from stdin only if the buffer is
 * empty.
 */

int get_request(

  char *request)  /* O */

  {
  static char line[MAX_LINE_LEN];  /* Stdin line */
  static char prompt[] = "Qmgr: ";
  /* Prompt if input is from terminal */
  static char contin[] = "Qmgr< ";
  /* Prompt if input is continued across lines */
  static int empty = TRUE;      /* Line has nothing in it */
  int eol;                      /* End of line */
  int ll;                     /* Length of line */
  int i = 0;                  /* Index into line */
  char *rp;                   /* Pointer into request */
  char *lp;                   /* Pointer into line */
  int eoc;                    /* End of command */
  char quote;                 /* Either ' or " */
  char *rlread = NULL;

  /* Make sure something is in the stdin line */

  if (empty != 0)
    {
    eol = FALSE;
    lp = line;
    ll = 0;

    while (eol == FALSE)
      {
#ifdef HAVE_READLINE

      if (rlread != NULL)
        {
        free(rlread);

        rlread = NULL;
        }

      if (isatty(0) && isatty(1))
        {
        if (lp == line)
          rlread = readline(prompt);
        else
          rlread = readline(contin);
        }
      else
        {
        rlread = readline("");
        }

      if (rlread == NULL)
        {
        return(EOF);
        }

      if ((*rlread != '\0') && (*rlread != '#'))
        {
        int blen  = strlen(rlread);
        int bsize = sizeof(line) - (lp - line);

        add_history(rlread);

        /* copy string plus termination marker */

        memcpy(
          lp,
          rlread,
          (blen < bsize) ? blen + 1 : bsize);
        }  /* END if ((*rlread != '\0') ... ) */

      line[sizeof(line) - 1] = '\0';

#else /* HAVE_READLINE */
      if (isatty(0) && isatty(1))
        {
        if (lp == line)
          printf("%s", prompt);
        else
          printf("%s", contin);
        }

      if (fgets(lp, sizeof(line) - ll, stdin) == NULL)
        {
        return(EOF);
        }

#endif /* HAVE_READLINE */

      ll = strlen(line);

      if (line[ll - 1] == '\n')
        {
        /* remove newline */

        line[ll - 1] = '\0';

        ll--;
        }

      lp = line; /* remove blank lines */

      while (White(*lp))
        lp++;

      if (strlen(lp) == 0)
        continue;

      if (line[ll - 1] == '\\')
        {
        line[ll - 1] = ' ';
        lp = &line[ll];
        }
      else if (*lp != '#')
        {
        eol = TRUE;
        }
      }
    }    /* END if (empty != 0) */

  /* Move a command from line to request */

  rp = request;

  lp = line;

  eoc = FALSE;

  while (!eoc)
    {
    switch (*lp)
      {
        /* End of command */

      case ';':

      case '#':

      case '\0':

        eoc = TRUE;

        break;

        /* Quoted string */

      case '"':

      case '\'':

        quote = *lp;
        *rp = *lp;

        rp++;
        lp++;

        while (*lp != quote && !EOL(*lp))
          {
          *rp = *lp;
          rp++;
          lp++;
          }

        *rp = *lp;

        rp++;
        lp++;
        break;

        /* Move the character */

      default:
        *rp = *lp;
        rp++;
        lp++;
        break;
      }
    }

  *rp = '\0';

  /* Is there any thing left in the line? */

  switch (*lp)
    {

    case '\0':

    case '#':
      i = 0;
      empty = TRUE;
      break;

    case ';':
      rp = line;
      lp++;

      while (White(*lp))
        lp++;

      if (!EOL(*lp))
        {
        strcpy(rp, lp);
        i = strlen(lp);
        empty = FALSE;
        }
      else
        {
        i = 0;
        empty = TRUE;
        }

      break;
    }

  for (;i < MAX_LINE_LEN;i++)
    line[i] = '\0';

  if (rlread != NULL)
    free(rlread);

  return(0);
  }  /* END get_request() */





/*
 *  is_attr - is the name passed an attribute
 *
 *    object       Type of object
 *    name         Name of the attribute
 *   attr_type    Is the attribute a cirtain type
 *
 *  Returns:
 *        This routine returns True if the input is a attribute of the
 *        given object and type.  Otherwise, it returns False.
 */

int is_attr(

  int   object,
  char *name,
  int   attr_type)

  {
  static char *svr_public_attrs[] =
    {
#include "qmgr_svr_public.h"
#include "site_qmgr_svr_print.h"
    NULL
    };

  static char *svr_readonly_attrs[] =
    {
#include "qmgr_svr_readonly.h"
    NULL
    };

  static char *que_public_attrs[] =
    {
#include "qmgr_que_public.h"
#include "site_qmgr_que_print.h"
    NULL
    };

  static char *que_readonly_attrs[] =
    {
#include "qmgr_que_readonly.h"
    NULL
    };

  static char *node_public_attrs[] =
    {
#include "qmgr_node_public.h"
#include "site_qmgr_node_print.h"
    NULL
    };

  static char *node_readonly_attrs[] =
    {
#include "qmgr_node_readonly.h"
    NULL
    };

  /* DIAGTODO: need a list of public and read-only diag attrs */
  char **attr_public = NULL;
  char **attr_readonly = NULL;

  char  *dupname  = NULL;
  char  *name_ptr = NULL;

  int    ret = FALSE;

  if (object == MGR_OBJ_SERVER)
    {
    attr_public = svr_public_attrs;
    attr_readonly = svr_readonly_attrs;
    }
  else if (object == MGR_OBJ_QUEUE)
    {
    attr_public = que_public_attrs;
    attr_readonly = que_readonly_attrs;
    }
  else if (object == MGR_OBJ_NODE)
    {
    attr_public = node_public_attrs;
    attr_readonly = node_readonly_attrs;
    }

  dupname = strdup(name);
  if (dupname == NULL)
    {
    return(FALSE);
    }

  name_ptr = strchr(dupname,'.');
  if (name_ptr != NULL)
    {
    *name_ptr = '\0';
    }

  if ((attr_public != NULL) &&
      (attr_type & TYPE_ATTR_PUBLIC))
    {
    while (*attr_public != NULL && ret == FALSE)
      {
      if (strcmp(dupname, *attr_public) == 0)
        {
        ret =  TRUE;
        }

      attr_public++;
      }
    }

  if ((attr_readonly != NULL) &&
      (attr_type & TYPE_ATTR_READONLY))
    {
    while (*attr_readonly != NULL && ret == FALSE)
      {
      if (strcmp(dupname, *attr_readonly) == 0)
        ret = TRUE;

      attr_readonly++;
      }
    }

  free(dupname);

  return(ret);
  }  /* END is_attr() */





/*
 *
 * show_help - show help for qmgr
 *
 *   str - possible sub topic to show help on
 *
 * returns nothing
 *
 */

void show_help(

  char *str)

  {
  while (White(*str))
    str++;

  if (str[0] == '\0')
    printf(HELP_DEFAULT);
  else if (strncmp(str, "active", 6) == 0)
    printf("%s%s", HELP_ACTIVE, HELP_ACTIVE2);
  else if (strncmp(str, "create", 6) == 0)
    printf(HELP_CREATE);
  else if (strncmp(str, "delete", 6) == 0)
    printf(HELP_DELETE);
  else if (strncmp(str, "set", 3) == 0)
    printf("%s%s", HELP_SET, HELP_SET2);
  else if (strncmp(str, "unset", 5) == 0)
    printf(HELP_UNSET);
  else if (strncmp(str, "list", 4) == 0)
    printf(HELP_LIST);
  else if (strncmp(str, "print", 5) == 0)
    printf(HELP_PRINT);
  else if (strncmp(str, "quit", 4) == 0)
    printf(HELP_QUIT);
  else if (strncmp(str, "exit", 4) == 0)
    printf(HELP_EXIT);
  else if (strncmp(str, "operator", 8) == 0)
    printf(HELP_OPERATOR);
  else if (strncmp(str, "value", 5) == 0)
    printf(HELP_VALUE);
  else if (strncmp(str, "name", 4) == 0)
    printf(HELP_NAME);
  else if (strncmp(str, "attribute", 9) == 0)
    printf(HELP_ATTRIBUTE);
  else if (strncmp(str, "serverpublic", 12) == 0)
    printf("%s%s%s", HELP_SERVERPUBLIC, HELP_SERVERPUBLIC2, HELP_SERVERPUBLIC3);
  else if (strncmp(str, "serverro", 8) == 0)
    printf(HELP_SERVERRO);
  else if (strncmp(str, "queuepublic", 11) == 0)
    printf("%s%s%s", HELP_QUEUEPUBLIC, HELP_QUEUEPUBLIC2, HELP_QUEUEPUBLIC3);
  else if (strncmp(str, "queueexec", 9) == 0)
    printf(HELP_QUEUEEXEC);
  else if (strncmp(str, "queueroute", 10) == 0)
    printf(HELP_QUEUEROUTE);
  else if (strncmp(str, "queuero", 7) == 0)
    printf(HELP_QUEUERO);
  else if (strncmp(str, "nodeattr", 8) == 0)
    printf(HELP_NODEATTR);
  else
    printf("No help available on: %s\nCheck the ERS for more help.\n", str);

  printf("\n");

  return;
  }  /* END show_help() */





/*
 *  parse - parse the qmgr request
 *
 *    request      The text of a single qmgr command.
 *    OUT: oper    Indicates either create, delete, set, unset, or list
 *    OUT: type    Indicates either server or queue.
 *    OUT: names   The names of the objects.
 *      OUT: attr    The attribute list with operators.
 *
 *  Returns:
 *        This routine returns zero upon successful completion.  If there is
 *        a syntax error in the command a non-zero value is returned.
 *
 * Note:
 *  The syntax of a qmgr directive is:
 *
 *      operation type [namelist] [attributelist]
 *
 *  where
 *      operation       create, delete, set, unset, list or print
 *      type            server or queue
 *      namelist        comma delimit list of names with no white space,
 *                      can only be defaulted if the type is server
 *      attributelist   comma delimit list of name or name-value pairs
 *
 *  If the operation part is quit or exit, then the code will be stopped
 *  now.
 */

int parse(

  char            *request,
  int             *oper,
  int             *type,
  char           **names,
  struct attropl **attr)

  {
  int         error;
  int         lp;       /* Length of current string */
  int         len;      /* ammount parsed by parse_request */
  int         i;        /* loop var */
  static char req[MAX_REQ_WORDS][MAX_REQ_WORD_LEN] = { {'\0'} };

  /* clear old data in req */

  for (i = IND_FIRST; i <= IND_LAST; i++)
    req[i][0] = '\0';

  /* parse the request into parts */

  len = parse_request(request, req);

  if (len != 0)
    {
    lp = strlen(req[IND_CMD]);

    if (strncmp(req[0], "create", lp) == 0)
      *oper = MGR_CMD_CREATE;
    else if (strncmp(req[0], "delete", lp) == 0)
      *oper = MGR_CMD_DELETE;
    else if (strncmp(req[0], "set", lp) == 0)
      *oper = MGR_CMD_SET;
    else if (strncmp(req[0], "unset", lp) == 0)
      *oper = MGR_CMD_UNSET;
    else if (strncmp(req[0], "list", lp) == 0)
      *oper = MGR_CMD_LIST;
    else if (strncmp(req[0], "print", lp) == 0)
      *oper = MGR_CMD_PRINT;
    else if (strncmp(req[0], "active", lp) == 0)
      *oper = MGR_CMD_ACTIVE;
    else if (strncmp(req[0], "help", lp) == 0)
      {
      show_help(req[1]);

      return(-1);
      }
    else if (strncmp(req[0], "quit", lp) == 0)
      clean_up_and_exit(0);
    else if (strncmp(req[0], "exit", lp) == 0)
      clean_up_and_exit(0);
    else
      {
      if (! zopt) fprintf(stderr, "qmgr: Illegal operation: %s\n"
                            "Try 'help' if you are having trouble.\n",
                            req[IND_CMD]);

      return(1);
      }

    if (EOL(req[IND_OBJ][0]))
      {
      if (! zopt) fprintf(stderr, "qmgr: No object type given\n");

      return(2);
      }

    lp = strlen(req[IND_OBJ]);

    if (strncmp(req[1], "server", lp) == 0)
      *type = MGR_OBJ_SERVER;
    else if (strncmp(req[1], "queue", lp) == 0)
      *type = MGR_OBJ_QUEUE;
    else if (strncmp(req[1], "node", lp) == 0)
      *type = MGR_OBJ_NODE;

    /* DIAGTODO: allow new object type */
    else
      {
      if (! zopt) fprintf(stderr, "qmgr: Illegal object type: %s.\n",
                            req[IND_OBJ]);

      return(2);
      }

    if (!EOL(req[IND_NAME][0]))
      {
      if (is_attr(*type, req[IND_NAME], TYPE_ATTR_ALL))
        {
        len -= strlen(req[IND_NAME]);

        req[IND_NAME][0] = '\0';
        }
      else
        {
        error = check_list(req[IND_NAME]);

        if (error != 0)
          {
          if (! zopt) fprintf(stderr, "qmgr: syntax error - checklist failed\n");

          CaretErr(request, len - (int)strlen(req[IND_NAME]) + error - 1);

          return(3);
          }
        }

      *names = req[IND_NAME];
      }

    /* get attribute list/verify operation is feasible */
    /* load remaining part of the request */

    if ((error = attributes(request + len, attr, *oper)) != 0)
      {
      if (! zopt) fprintf(stderr, "qmgr: Syntax error - cannot locate attribute\n");

      CaretErr(request, len + error);

      return(4);
      }

    if (((*oper == MGR_CMD_SET) || (*oper == MGR_CMD_UNSET)) && (*attr == NULL))
      {
      if (! zopt) fprintf(stderr, "qmgr: Syntax error - attribute required for operation\n");

      CaretErr(request, len + error);

      return(4);
      }

    if ((*oper == MGR_CMD_ACTIVE) && (*attr != NULL))
      {
      if (! zopt) fprintf(stderr, "qmgr: Syntax error - operation does not allow attribute\n");

      CaretErr(request, len);

      return(4);
      }
    }    /* END if (len != 0) */

  return(0);
  }  /* END parse() */






/*
 *  pstderr_big - prints error message to standard error.  It handles
 *                    the highly unusual case where the error message
 *                    that's to be generated is too big to be placed in
 *                    the buffer that was allocated.  In this case the
 *                    message is put out in pieces to stderr.  Kind of
 *                    ugly, but one doesn't really expect this code to be
 *                    called except in the oddest of cases.
 *
 *  svrname       name of the server
 *  objname       name of the object
 *  errmesg       actual error message
 *
 *  Returns:
 *       Nothing.
 *
 *  Global Variable: zopt
 */

void pstderr_big(

  char *svrname,
  char *objname,
  char *errmesg)

  {
  if (! zopt)
    {
    fprintf(stderr, "qmgr obj=");
    fprintf(stderr, "%s", objname);
    fprintf(stderr, " svr=");
    fprintf(stderr, "%s", svrname);
    fprintf(stderr, ": ");
    fprintf(stderr, "%s", errmesg);
    fprintf(stderr, "\n");
    }

  return;
  }


/*
 *
 * free_objname_list - frees an objname list
 *
 *   list - objname list to free
 *
 * returns nothing
 *
 */
void
free_objname_list(struct objname *list)
  {

  struct objname *cur, *tmp;

  cur = list;

  while (cur != NULL)
    {
    tmp = cur -> next;
    free_objname(cur);
    cur = tmp;
    }
  }

/*
 *
 * find_server - find a server in the server list
 *
 *   name - the name of the server
 *
 * returns a pointer to the specified server struct or NULL if not found
 *
 */

struct server *find_server(char *name)
  {

  struct server *s = NULL;

  if (name != NULL)
    {
    s = servers;

    while (s != NULL && strcmp(s -> s_name, name))
      s = s -> next;
    }

  return s;
  }

/*
 *
 * new_server - allocate new server objcet and initialize it
 *
 * returns new server object
 *
 */

struct server *
      new_server(void)
  {

  struct server *new;

  Mstruct(new, struct server);
  new -> s_connect = -1;
  new -> s_name = NULL;
  new -> ref = 0;
  new -> next = NULL;
  return new;
  }

/*
 *
 * free_server - remove server from servers list and free it up
 *
 *   svr - the server to free
 *
 * returns nothing
 *
 */
void
free_server(struct server *svr)
  {

  struct server *cur_svr, *prev_svr = NULL;

  /* remove server from servers list */
  cur_svr = servers;

  while (cur_svr != svr && cur_svr != NULL)
    {
    prev_svr = cur_svr;
    cur_svr = cur_svr -> next;
    }

  if (cur_svr != NULL)
    {
    if (prev_svr == NULL)
      servers = servers -> next;
    else
      prev_svr -> next = cur_svr -> next;

    if (svr -> s_name != NULL)
      free(svr -> s_name);

    svr -> s_name = NULL;

    svr -> s_connect = -1;

    svr -> next = NULL;

    free(svr);
    }
  }


/*
 *
 * new_objname - allocate new object and initialize it
 *
 * returns newly allocated object
 *
 */

struct objname *
      new_objname(void)
  {

  struct objname *new;
  Mstruct(new, struct objname);
  new -> obj_type = MGR_OBJ_NONE;
  new -> obj_name = NULL;
  new -> svr_name = NULL;
  new -> svr = NULL;
  new -> next = NULL;

  return new;
  }

/*
 *
 * free_objname - frees space used by an objname
 *
 *   obj - objname to free
 *
 * returns nothing
 *
 */
void
free_objname(struct objname *obj)
  {
  if (obj -> obj_name != NULL)
    free(obj -> obj_name);

  if (obj -> obj_type != MGR_OBJ_SERVER && obj -> svr_name != NULL)
    free(obj -> svr_name);

  if (obj -> svr != NULL)
    obj -> svr -> ref--;

  obj -> svr = NULL;

  obj -> obj_name = NULL;

  obj -> svr_name = NULL;

  obj -> next = NULL;

  free(obj);
  }

/*
 *
 * strings2objname - convert an array of strings to a list of objnames
 *
 *   str - array of strings
 *   num - number of strings
 *   type  - type of objects
 *
 * returns newly allocated list of objnames
 *
 */

struct objname *
      strings2objname(char **str, int num, int type)
  {

  struct objname *objs = NULL;  /* head of objname list */

  struct objname *cur_obj;  /* current object in objname list */

  struct objname *prev_obj = NULL; /* previous object in objname list */
  int i;
  int len;

  if (str != NULL)
    {
    for (i = 0; i < num; i++)
      {
      cur_obj = new_objname();

      len = strlen(str[i]);
      Mstring(cur_obj -> obj_name, len + 1);
      strcpy(cur_obj -> obj_name, str[i]);
      cur_obj -> obj_type = type;

      if (type == MGR_OBJ_SERVER)
        cur_obj -> svr_name = cur_obj -> obj_name;

      if (prev_obj != NULL)
        prev_obj -> next = cur_obj;

      if (objs == NULL)
        objs = cur_obj;

      prev_obj = cur_obj;
      }
    }

  return objs;
  }

/*
 *
 * is_valid_object - connects to the server to check if the object exists
 *     on the server its connected.
 *
 *   obj - object to check
 *   type - type of object
 *
 *
 * returns 1 if its valid, 0 if its not
 *
 */

int is_valid_object(

  struct objname *obj,
  int             type)

  {

  struct batch_status *batch_obj = NULL;
  /* we need something to make the pbs_stat* call.
   * Even if we only want the object name
   */

  static struct attrl attrq =
    {
    NULL, ATTR_qtype, "", "", 0
    };

  static struct attrl attrn =
    {
    NULL, ATTR_NODE_state, "", "", 0
    };

  int   local_errno = 0;
  int   valid = 1;
  char *errmsg;

  if ((obj != NULL) && (obj->svr != NULL))
    {
    switch (type)
      {

      case MGR_OBJ_QUEUE:

        batch_obj = pbs_statque_err(obj->svr->s_connect, obj->obj_name, &attrq, NULL, &local_errno);

        break;

      case MGR_OBJ_NODE:

        batch_obj = pbs_statnode_err(obj->svr->s_connect, obj->obj_name, &attrn, NULL, &local_errno);

        break;

      default:

        valid = 0;

        break;
      }

    if (batch_obj == NULL)
      {
      errmsg = pbs_geterrmsg(obj->svr->s_connect);

      if (! zopt) fprintf(stderr, "qmgr: %s.\n",
                            errmsg);

      valid = 0;
      }
    else
      {
      /* if pbs_stat*() returned something, then the object exists */

      valid = 1;

      pbs_statfree(batch_obj);
      }
    }
  else
    {
    valid = 1; /* NULL server means all active servers */
    }

  return(valid);
  }





/*
 *
 * default_server_name - create an objname struct for the default server
 *
 * returns newly allocated objname with the default server assigned
 *
 */

struct objname *
      default_server_name(void)

  {

  struct objname *obj;

  obj = new_objname();
  /* default server name is the NULL string */
  Mstring(obj -> obj_name, 1);
  obj->obj_name[0] = '\0';
  obj->svr_name = obj->obj_name;
  obj->obj_type = MGR_OBJ_SERVER;

  return(obj);
  }





/*
 *
 * temp_objname - set up a temporary objname struct.  This is meant for
 *         a one time use.  The memory is static.
 *
 *   obj_name - name for temp struct
 *   svr_name - name of the server for the temp struct
 *   svr  - server for temp struct
 *
 * returns temporary objname
 *
 */

struct objname *temp_objname(

        char *obj_name,
        char *svr_name,
        struct server *svr)

  {

  static struct objname temp =
    {
    0, NULL, NULL, NULL, NULL
    };

  if (temp.svr != NULL)
    temp.svr->ref--;

  temp.obj_name = NULL;

  temp.svr_name = NULL;

  temp.svr = NULL;

  temp.obj_name = obj_name;

  temp.svr_name = svr_name;

  temp.svr = svr;

  if (temp.svr != NULL)
    temp.svr->ref++;

  return(&temp);
  }

/*
 *
 * close_non_ref_servers - close all nonreferenced servers
 *
 * returns nothing
 *
 */

void
close_non_ref_servers(void)

  {

  struct server *svr, *tmp_svr;

  svr = servers;

  while (svr != NULL)
    {
    tmp_svr = svr -> next;

    if (svr -> ref == 0)
      disconnect_from_server(svr);

    svr = tmp_svr;
    }

  return;
  }   /* END close_non_ref_servers() */






/*
 *
 * parse_request - parse out the command, object, and possible name
 *
 * cases:
 *  FULL: command object name ...
 *        command object ...
 * NOTE: there does not need to be whitespace around the operator
 *
 *   request - the request to be processed
 *   OUT: req - array to return data in
 *   indicies:
 *   IND_CMD   : command
 *   IND_OBJ   : object
 *   IND_NAME  : name
 *
 *   if any field is not there, it is left blank.
 *
 * returns The number of characters parsed.  Note: 0 chars parsed is error
 *  Data is passed back via the req variable.
 */

int parse_request(

  char *request,
  char  req[][MAX_REQ_WORD_LEN])

  {
  char *foreptr;
  char *backptr;
  int   len = 0;
  int   i = 0;
  int   chars_parsed = 0;
  int   error = 0;

  foreptr = request;

  for (i = 0; !EOL(*foreptr) && (i < MAX_REQ_WORDS) && (error == 0);)
    {
    while (White(*foreptr))
      foreptr++;

    backptr = foreptr;

    while (!White(*foreptr) && !Oper(foreptr) && !EOL(*foreptr))
      foreptr++;

    len = foreptr - backptr;

    if (len >= MAX_REQ_WORD_LEN)
      {
      error = 1;
      chars_parsed = (int)(foreptr - request);

      if (!zopt)
        fprintf(stderr, "qmgr: max word length exceeded\n");

      CaretErr(request, chars_parsed);

      return(error);
      }

    strncpy(req[i], backptr, len);
    req[i][len] = '\0';

    i++;
    }  /* END for (i) */

  chars_parsed = foreptr - request;

  /* SUCCESS */
  return(chars_parsed);
  } /* END parse_request() */


/* END qmgr.c */
