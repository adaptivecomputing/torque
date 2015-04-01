include $(top_srcdir)/buildutils/config.mk

noinst_LTLIBRARIES = libscaffold_fail.la

libscaffold_fail_la_SOURCES = 	account_jobend.cpp \
								cleanup_restart_file.cpp \
								close_conn.cpp \
								destroy_alps_reservation.cpp \
								enqueue_threadpool_request.cpp \
								get_jobs_array.cpp \
								get_jobs_queue.cpp \
								insert_task.cpp \
								job_abt.cpp \
								modify_job_attr.cpp \
								next_job.cpp \
								pbse_to_txt.cpp \
								release_req.cpp \
								remove_alps_reservation.cpp \
								remove_job_delete_nanny.cpp \
								reply_ack.cpp \
								req_reject.cpp \
								site_alt_router.cpp \
								svr_disconnect.cpp \
								svr_evaljobstate.cpp \
								svr_find_job.cpp \
								svr_movejob.cpp \
								threadsafe_tokenizer.cpp \
								unlock_queue.cpp \
								update_array_values.cpp

libscaffold_fail_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov
