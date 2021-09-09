/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#ifndef _SERVIDORALMACENAMIENTO_H_RPCGEN
#define _SERVIDORALMACENAMIENTO_H_RPCGEN

#include <rpc/rpc.h>

#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif


struct storemessage_1_argument {
	char *topic;
	char *msg;
};
typedef struct storemessage_1_argument storemessage_1_argument;

#define SERVIDOR_ALMACENAMIENTO 99
#define PRIMERA_VERSION 1

#if defined(__STDC__) || defined(__cplusplus)
#define init 1
extern  enum clnt_stat init_1(int *, CLIENT *);
extern  bool_t init_1_svc(int *, struct svc_req *);
#define storeMessage 2
extern  enum clnt_stat storemessage_1(char *, char *, int *, CLIENT *);
extern  bool_t storemessage_1_svc(char *, char *, int *, struct svc_req *);
#define getLastMessage 3
extern  enum clnt_stat getlastmessage_1(char *, char **, CLIENT *);
extern  bool_t getlastmessage_1_svc(char *, char **, struct svc_req *);
extern int servidor_almacenamiento_1_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define init 1
extern  enum clnt_stat init_1();
extern  bool_t init_1_svc();
#define storeMessage 2
extern  enum clnt_stat storemessage_1();
extern  bool_t storemessage_1_svc();
#define getLastMessage 3
extern  enum clnt_stat getlastmessage_1();
extern  bool_t getlastmessage_1_svc();
extern int servidor_almacenamiento_1_freeresult ();
#endif /* K&R C */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t xdr_storemessage_1_argument (XDR *, storemessage_1_argument*);

#else /* K&R C */
extern bool_t xdr_storemessage_1_argument ();

#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#endif /* !_SERVIDORALMACENAMIENTO_H_RPCGEN */
