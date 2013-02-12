include $(top_srcdir)/buildutils/config.mk

noinst_LTLIBRARIES = libscaffold_fail.la

libscaffold_fail_la_SOURCES = 	account_jobend.c \
								cleanup_restart_file.c \
								close_conn.c \
								destroy_alps_reservation.c \
								enqueue_threadpool_request.c \
								get_jobs_array.c \
								get_jobs_queue.c \
								insert_task.c \
								job_abt.c \
								modify_job_attr.c \
								next_job.c \
								pbse_to_txt.c \
								release_req.c \
								remove_alps_reservation.c \
								remove_job_delete_nanny.c \
								reply_ack.c \
								req_reject.c \
								site_alt_router.c \
								svr_disconnect.c \
								svr_evaljobstate.c \
								svr_find_job.c \
								svr_movejob.c \
								threadsafe_tokenizer.c \
								unlock_queue.c \
								update_array_values.c

libscaffold_fail_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov
