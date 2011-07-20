# 1 "d:\\loadrunner\\echo\\\\combined_echo.c"
# 1 "C:\\Program Files\\Mercury Interactive\\Mercury LoadRunner\\include/lrun.h" 1
 












 











# 102 "C:\\Program Files\\Mercury Interactive\\Mercury LoadRunner\\include/lrun.h"








































































	

 


















 
 
 
 
 


 
 
 
 
 
 














int     lr_start_transaction   (char * transaction_name);
int lr_start_sub_transaction          (char * transaction_name, char * trans_parent);
long lr_start_transaction_instance    (char * transaction_name, long parent_handle);



int     lr_end_transaction     (char * transaction_name, int status);
int lr_end_sub_transaction            (char * transaction_name, int status);
int lr_end_transaction_instance       (long transaction, int status);


 
typedef char* lr_uuid_t;
 



lr_uuid_t lr_generate_uuid();

 


int lr_generate_uuid_free(lr_uuid_t uuid);

 



int lr_generate_uuid_on_buf(lr_uuid_t buf);

   
# 262 "C:\\Program Files\\Mercury Interactive\\Mercury LoadRunner\\include/lrun.h"
int lr_start_distributed_transaction  (char * transaction_name, lr_uuid_t correlator, long timeout  );

   







int lr_end_distributed_transaction  (lr_uuid_t correlator, int status);


double lr_stop_transaction            (char * transaction_name);
double lr_stop_transaction_instance   (long parent_handle);


void lr_resume_transaction           (char * trans_name);
void lr_resume_transaction_instance  (long trans_handle);


int lr_update_transaction            (const char *trans_name);


 
void lr_wasted_time(long time);


 
int lr_set_transaction(const char *name, double duration, int status);
 
long lr_set_transaction_instance(const char *name, double duration, int status, long parent_handle);


int   lr_user_data_point                      (char *, double);
long lr_user_data_point_instance                   (char *, double, long);
 



int lr_user_data_point_ex(const char *dp_name, double value, int log_flag);
long lr_user_data_point_instance_ex(const char *dp_name, double value, long parent_handle, int log_flag);


int lr_transaction_add_info      (const char *trans_name, char *info);
int lr_transaction_instance_add_info   (long trans_handle, char *info);
int lr_dpoint_add_info           (const char *dpoint_name, char *info);
int lr_dpoint_instance_add_info        (long dpoint_handle, char *info);


double lr_get_transaction_duration       (char * trans_name);
double lr_get_trans_instance_duration    (long trans_handle);
double lr_get_transaction_think_time     (char * trans_name);
double lr_get_trans_instance_think_time  (long trans_handle);
double lr_get_transaction_wasted_time    (char * trans_name);
double lr_get_trans_instance_wasted_time (long trans_handle);
int    lr_get_transaction_status		 (char * trans_name);
int	   lr_get_trans_instance_status		 (long trans_handle);

 



int lr_set_transaction_status(int status);

 



int lr_set_transaction_status_by_name(int status, const char *trans_name);
int lr_set_transaction_instance_status(int status, long trans_handle);


typedef void* merc_timer_handle_t;
 

merc_timer_handle_t lr_start_timer();
double lr_end_timer(merc_timer_handle_t timer_handle);


 
 
 
 
 
 











 



int   lr_rendezvous  (char * rendezvous_name);
 




int   lr_rendezvous_ex (char * rendezvous_name);



 
 
 
 
 
char *lr_get_vuser_ip (void);
void   lr_whoami (int *vuser_id, char ** sgroup, int *scid);
char *	  lr_get_host_name (void);
char *	  lr_get_master_host_name (void);

 
long     lr_get_attrib_long	(char * attr_name);
char *   lr_get_attrib_string	(char * attr_name);
double   lr_get_attrib_double      (char * attr_name);


 
 
static void *ci_this_context = 0;






 








void lr_continue_on_error (int lr_continue);
char *   lr_decrypt (const char *EncodedString);


 
 
 
 
 
 



 






 














void   lr_abort (void);
void lr_exit(int exit_option, int exit_status);
void lr_abort_ex (unsigned long flags);

void   lr_peek_events (void);


 
 
 
 
 


void   lr_think_time (double secs);

 


void lr_force_think_time (double secs);


 
 
 
 
 



















int   lr_msg (char * fmt, ...);
int   lr_debug_message (unsigned int msg_class,
									    char * format,
										...);
# 492 "C:\\Program Files\\Mercury Interactive\\Mercury LoadRunner\\include/lrun.h"
void   lr_new_prefix (int type,
                                 char * filename,
                                 int line);
# 495 "C:\\Program Files\\Mercury Interactive\\Mercury LoadRunner\\include/lrun.h"
int   lr_log_message (char * fmt, ...);
int   lr_message (char * fmt, ...);
int   lr_error_message (char * fmt, ...);
int   lr_output_message (char * fmt, ...);
int   lr_vuser_status_message (char * fmt, ...);
int   lr_error_message_without_fileline (char * fmt, ...);
int   lr_fail_trans_with_error (char * fmt, ...);

 
 
 
 
 
# 518 "C:\\Program Files\\Mercury Interactive\\Mercury LoadRunner\\include/lrun.h"

 
 
 
 
 





int   lr_next_row ( char * table);
int lr_advance_param ( char * param);



														  
														  

														  
														  

													      
 


char *   lr_eval_string (char * str);
int   lr_eval_string_ext (const char *in_str,
                                     unsigned long const in_len,
                                     char ** const out_str,
                                     unsigned long * const out_len,
                                     unsigned long const options,
                                     const char *file,
								     long const line);
# 552 "C:\\Program Files\\Mercury Interactive\\Mercury LoadRunner\\include/lrun.h"
void   lr_eval_string_ext_free (char * * pstr);

 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
int lr_param_increment (char * dst_name,
                              char * src_name);
# 575 "C:\\Program Files\\Mercury Interactive\\Mercury LoadRunner\\include/lrun.h"













											  
											  

											  
											  
											  

int	  lr_save_var (char *              param_val,
							  unsigned long const param_val_len,
							  unsigned long const options,
							  char *			  param_name);
# 599 "C:\\Program Files\\Mercury Interactive\\Mercury LoadRunner\\include/lrun.h"
int   lr_save_string (const char * param_val, const char * param_name);


 
 
 
 
 
 
# 664 "C:\\Program Files\\Mercury Interactive\\Mercury LoadRunner\\include/lrun.h"
void   lr_save_datetime (const char *format, int offset, const char *name);









 











 
 
 
 
 






 



char * lr_error_context_get_entry (char * key);

 



long   lr_error_context_get_error_id (void);


 
 
 

int lr_table_get_rows_num (char * param_name);

int lr_table_get_cols_num (char * param_name);

char * lr_table_get_cell_by_col_index (char * param_name, int row, int col);

char * lr_table_get_cell_by_col_name (char * param_name, int row, const char* col_name);

int lr_table_get_column_name_by_index (char * param_name, int col, 
											char * * const col_name,
											int * col_name_len);
# 725 "C:\\Program Files\\Mercury Interactive\\Mercury LoadRunner\\include/lrun.h"

int lr_table_get_column_name_by_index_free (char * col_name);


 
 
 
 
 
 
 
 

 
 
 
 
 
 
int   lr_param_substit (char * file,
                                   int const line,
                                   char * in_str,
                                   int const in_len,
                                   char * * const out_str,
                                   int * const out_len);
# 750 "C:\\Program Files\\Mercury Interactive\\Mercury LoadRunner\\include/lrun.h"
void   lr_param_substit_free (char * * pstr);


 
# 762 "C:\\Program Files\\Mercury Interactive\\Mercury LoadRunner\\include/lrun.h"





char *   lrfnc_eval_string (char * str,
                                      char * file_name,
                                      long const line_num);
# 770 "C:\\Program Files\\Mercury Interactive\\Mercury LoadRunner\\include/lrun.h"


int   lrfnc_save_string ( const char * param_val,
                                     const char * param_name,
                                     const char * file_name,
                                     long const line_num);
# 776 "C:\\Program Files\\Mercury Interactive\\Mercury LoadRunner\\include/lrun.h"

int lr_save_searched_string(char *buffer, long buf_size, unsigned int occurrence,
			    char *search_string, int offset, unsigned int param_val_len, 
			    char *param_name);

 
char *   lr_string (char * str);

 
# 842 "C:\\Program Files\\Mercury Interactive\\Mercury LoadRunner\\include/lrun.h"

int   lr_save_value (char * param_val,
                                unsigned long const param_val_len,
                                unsigned long const options,
                                char * param_name,
                                char * file_name,
                                long const line_num);
# 849 "C:\\Program Files\\Mercury Interactive\\Mercury LoadRunner\\include/lrun.h"


 
 
 
 
 











int   lr_printf (char * fmt, ...);
 
int   lr_set_debug_message (unsigned int msg_class,
                                       unsigned int swtch);
# 871 "C:\\Program Files\\Mercury Interactive\\Mercury LoadRunner\\include/lrun.h"
unsigned int   lr_get_debug_message (void);


 
 
 
 
 

void   lr_double_think_time ( double secs);
void   lr_usleep (long);


 
 
 
 
 
 




int *   lr_localtime (long offset);


int   lr_send_port (long port);


# 947 "C:\\Program Files\\Mercury Interactive\\Mercury LoadRunner\\include/lrun.h"



struct _lr_declare_identifier{
	char signature[24];
	char value[128];
};

int   lr_pt_abort (void);

void vuser_declaration (void);






# 976 "C:\\Program Files\\Mercury Interactive\\Mercury LoadRunner\\include/lrun.h"


# 988 "C:\\Program Files\\Mercury Interactive\\Mercury LoadRunner\\include/lrun.h"
















 
 
 
 
 







int    _lr_declare_transaction   (char * transaction_name);


 
 
 
 
 







int   _lr_declare_rendezvous  (char * rendezvous_name);

 
 
 
 
 

 
int lr_enable_ip_spoofing();
int lr_disable_ip_spoofing();


 




int lr_convert_string_encoding(char *sourceString, char *fromEncoding, char *toEncoding, char *paramName);






# 1 "d:\\loadrunner\\echo\\\\combined_echo.c" 2

# 1 "vuser_init.c" 1
 





# 1 "C:\\Program Files\\Mercury Interactive\\Mercury LoadRunner\\include/lrs.h" 1



 
# 1 "C:\\Program Files\\Mercury Interactive\\Mercury LoadRunner\\include/lrs_err.h" 1






 
























 




 





 

 







 








































 










# 5 "C:\\Program Files\\Mercury Interactive\\Mercury LoadRunner\\include/lrs.h" 2
































































































 
 




 




 
enum{
	LRS_NO_DELAY
};

 
enum{
	LOCAL_ADDRESS,
	LOCAL_HOSTNAME,
	LOCAL_PORT,
	REMOTE_ADDRESS,
	REMOTE_HOSTNAME,
	REMOTE_PORT
};

 
enum{
	Mismatch,
	EndMarker
};
 
enum{
	MISMATCH_SIZE,
	MISMATCH_CONTENT
};
 
enum{
	EndMarker_None,
	StringTerminator,
	BinaryStringTerminator,
	RecordingSize
};

 
enum{
	RecieveOption_None = 0,
	NoOption = 0,
	OffsetSize = 1,
	LeftRightBoundaries = 2,
	LeftBoundarySize = 3
};


 
int		LrsStartup(int ver_info);
int		LrsCleanup(void);
int		LrsCreateSocket(char *s_desc, char *s_type, ...);
int		LrsAcceptConnection(char *old_s_desc, char *new_s_desc);
int		LrsCloseSocket(char	*s_desc);
int		LrsDisableSocket(char *s_desc, int operation_to_disable);
int		LrsSend(char *s_desc, char *buf_desc, ...);
int		LrsLengthSend(char *s_desc, char	*buf_desc, int send_option, ...);
int		LrsSetSendBuffer(char *s_desc, char *buffer, int size);
int		LrsReceive(char	*s_desc, char *buf_desc, ...);
int		LrsReceiveEx(char *s_desc, char	*buf_desc, ...);
int		LrsLengthReceive(char *s_desc, char	*buf_desc, int receive_option, ...);
int		LrsSetReceiveOption(int option,	int value, ...);
int		LrsGetSocketHandler(char *s_desc);
int		LrsSetSocketHandler(char *s_desc,int s_handler);
int		LrsGetLastReceivedBuffer(char *s_desc, char **actual_buf, int *actual_bytes);
char*	LrsGetReceivedBuffer(char* s_desc, int sub_buf_offset, int	sub_buf_len, char*	encoding);
int		LrsGetLastReceivedBufferSize(char*	s_desc);
int		LrsGetBufferByName(char	*buf_desc, char	**actual_buf, int *actual_bytes);
char*	LrsGetStaticBuffer(char	*s_desc,char* buf_desc,int sub_buf_offset,int sub_buf_len,char* encoding);
int		LrsSaveParam(char* s_desc, char* buf_desc, char* param_name, int offset, int param_len);
int		LrsSaveParamEx(char* s_desc, char* whence, char* buf_desc, int offset, int param_len, char*	encoding, char*	param_name);
int		LrsSaveSearchedString(char*	s_desc,	char* buf_desc, char* param_name, char*	left_boundary, char* right_boundary,int ordinal, int offset, int param_len);
int		LrsFreeBuffer(char*	buffer);
void	LrsSetSendTimeout(long	tv_sec, long tv_usec);
void	LrsSetRecvTimeout(long	tv_sec,long	tv_usec);
void	LrsSetRecvTimeout2(long	tv_sec,long	tv_usec);
void	LrsSetAcceptTimeout(long tv_sec, long tv_usec);
void	LrsSetConnectTimeout(long	tv_sec,long	tv_usec);
char*	LrsEbcdicToAscii(char* s_desc, char* buf, long buf_length);
char*	LrsAsciiToEbcdic(char* s_desc, char* buf, long buf_length);
char*	LrsDecimalToHexString(char*	s_desc,	char* buf, long buf_length);
int		LrsHexStringToInt(char*	buf, long buf_length, int* mpiOutput);
char*	LrsGetUserBuffer(char* s_desc);
long	LrsGetUserBufferSize(char* s_desc);
int		LrsSetSocketOptions(char	*s_desc, int option, char *option_val);
char*	LrsGetSocketAttrib(char *s_desc, int attrib);
int		LrsExcludeSocket(char* s_desc);



# 7 "vuser_init.c" 2



vuser_init()
{
    LrsStartup(257);
	LrsCreateSocket("socket0", "TCP", "RemoteHost=192.168.1.252:1980",  "0");

    return 0;
}

# 2 "d:\\loadrunner\\echo\\\\combined_echo.c" 2

# 1 "Action.c" 1
 








Action()
{
    

    LrsSend("socket0", "buf0", "0");

    LrsReceive("socket0", "buf1", "0");

    return 0;
}

# 3 "d:\\loadrunner\\echo\\\\combined_echo.c" 2

# 1 "vuser_end.c" 1
 








vuser_end()
{
    LrsCleanup();

    return 0;
}

# 4 "d:\\loadrunner\\echo\\\\combined_echo.c" 2

