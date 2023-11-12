double atof( const char *string);
char *strdup( const char *string);
char *strcat( char *to, const char *from );
char *strncpy( char *dest, const char *source, size_t n ); 

int Is_influx_enabled = 0;
int Is_influx_auth_enabled = 0;
char* Influx_URL;
char* Influx_DB;
char* Influx_Measurement;
char* Influx_Precision;
char* Influx_User;
char* Influx_Password;
int test_i=0;

typedef struct Field_map {
	char *Field;
	char *Param;
	char* FType;
	struct Field_map *next;	
}Field_map;

int Field_map_array_length=0;
int Field_map_array_size=0;
int Field_map_array_max_element_size=0;
Field_map* Field_map_array=NULL;

typedef struct Param_map {
	char *tag;
	char *param;
	struct Param_map *next;	
}Param_map;

int Param_map_array_length=0;
int Param_map_array_size=0;
int Param_map_array_max_element_size=0;
Param_map* Param_map_array=NULL;



char *strReplace(const char *src, const char *from, const char *to)
{
  char *value;
  char *dst;
  char *match;
  int size;
  int fromlen;
  int tolen;
 
  // Find out the lengths of the source string, text to replace, and
  // the replacement text.
  size = strlen(src) + 1;
  fromlen = strlen(from);
  tolen = strlen(to);
 
  // Allocate the first chunk with enough for the original string.
  value = (char *)malloc(size);
 
  // We need to return 'value', so let's make a copy to mess around with.
  dst = value;
 
  // Before we begin, let's see if malloc was successful.
  if ( value != NULL )
  {
    // Loop until no matches are found.
    for ( ;; )
    {
      // Try to find the search text.
      match = (char *) strstr(src, from);
      if ( match != NULL )
      {
        // Found search text at location 'match'.
        // Find out how many characters to copy up to the 'match'.
        size_t count = match - src;
 
        // We are going to realloc, and for that we will need a
        // temporary pointer for safe usage.
        char *temp;
 
        // Calculate the total size the string will be after the
        // replacement is performed.
        size += tolen - fromlen;
 
        // Attempt to realloc memory for the new size.
        //
        // temp = realloc(value, size);
        temp = (char *)realloc(value, size);
 
        if ( temp == NULL )
        {
          // Attempt to realloc failed. Free the previously malloc'd
          // memory and return with our tail between our legs.
          free(value);
          return NULL;
        }
 
        // The call to realloc was successful. But we'll want to
        // return 'value' eventually, so let's point it to the memory
        // that we are now working with. And let's not forget to point
        // to the right location in the destination as well.
        dst = temp + (dst - value);
        value = temp;
 
        // Copy from the source to the point where we matched. Then
        // move the source pointer ahead by the amount we copied. And
        // move the destination pointer ahead by the same amount.
        memmove(dst, src, count);
        src += count;
        dst += count;
 
        // Now copy in the replacement text 'to' at the position of
        // the match. Adjust the source pointer by the text we replaced.
        // Adjust the destination pointer by the amount of replacement
        // text.
        memmove(dst, to, tolen);
        src += fromlen;
        dst += tolen;
      }
      else // No match found.
      {
        // Copy any remaining part of the string. This includes the null
        // termination character.
        strcpy(dst, src);
        break;
      }
    } // For Loop()
  }
  return value;
}

// ----------------------------------------------------------------------------
//
// Description:
//    escape symbols ' ' '=' ','  with '\' in string.
//
// Parameters:
//    P_string (in)    - pointer to string for processing
//    
// Returns:
//    Returns a pointer to string.
//
// ----------------------------------------------------------------------------
char* InfluxEscapeSymbols (const char* P_string){
	char *tmp_str=(char*)malloc((strlen(P_string)*3+1)*sizeof(char));
	if(P_string==NULL){
		return "";
	}
	tmp_str=strReplace(P_string, ",", "\\,");
	tmp_str=strReplace(tmp_str, " ", "\\ ");
	tmp_str=strReplace(tmp_str, "=", "\\=");
	tmp_str=strReplace(tmp_str, "\"", "\\\"");
	return tmp_str;
}
 
// ----------------------------------------------------------------------------
//
// Description:
//    Find and replace text within a LoadRunner string.
//
// Parameters:
//    lrparam (in)    - pointer to LoadRunner Parameter Name
//    findstr (in)    - pointer to text top search for
//    replacestr (in) - pointer to text to use as replacement
//
// Returns:
//    Returns an integer. -1=Error, 0=Success.
//
// Example:
//    lr_save_string( "This is a small test of the search and replace function", "LRParam");
//    lr_replace( "LRParam", "a", "-x-" );
//    lr_message( "%s", lr_eval_string("{LRParam}") );
//
// ----------------------------------------------------------------------------
int lr_replace( const char *lrparam, char *findstr, char *replacestr )
{
  int res = -1;
  char *result_str;
  char lrp[1024];
 
  // Finalize the LR Param Name
  sprintf( lrp, "{%s}", lrparam);
 
  // Do the Search and Replace
  result_str = strReplace( lr_eval_string(lrp), findstr, replacestr );
 
  // Process results
  if (result_str != NULL )
  {
    lr_save_string( result_str, lrparam );
    free( result_str );
    res = 0;
  }
  return res;
} // EOF


char* InfluxEncodeString(char* source, char* str_from, char* str_to){
	char * tmp_str;
	
	lr_convert_string_encoding(source,str_from,str_to,"temp_param_for_string_encode");
	tmp_str=(char*)malloc((strlen(lr_eval_string("{temp_param_for_string_encode}"))+1)*sizeof(char));
 	sprintf(tmp_str,"%s",lr_eval_string("{temp_param_for_string_encode}"));
	return tmp_str;
}

void InfluxDefine (char* P_Influx_URL,char* P_Influx_DB,char* P_Influx_Measurement){
	Is_influx_enabled = 1;
	Influx_URL=(char*)malloc((strlen(P_Influx_URL)+1)*sizeof(char));
	Influx_DB=(char*)malloc((strlen(P_Influx_DB)+1)*sizeof(char));
	Influx_Measurement=(char*)malloc((strlen(P_Influx_Measurement)+1)*sizeof(char));
	sprintf(Influx_URL,"%s",P_Influx_URL);
	sprintf(Influx_DB,"%s",P_Influx_DB);
	sprintf(Influx_Measurement,"%s",P_Influx_Measurement);
}

int InfluxDefineAuth (char* P_Influx_User,char* P_Influx_Password){
	Influx_User=(char*)malloc((strlen(P_Influx_User)+1)*sizeof(char));
	Influx_Password=(char*)malloc((strlen(P_Influx_Password)+1)*sizeof(char));
	Is_influx_auth_enabled=1;
	sprintf(Influx_User,"%s",P_Influx_User);
	sprintf(Influx_Password,"%s",P_Influx_Password);	
	return 0;	
}

void InfluxDefinePrecision (char* P_Influx_Precision){
	Influx_Precision=(char*)malloc((strlen(P_Influx_Precision)+1)*sizeof(char));
	sprintf(Influx_Precision,"%s",P_Influx_Precision);
}

int InfluxDefineFile (char* filename){
	int i,j,ret=0;
	int LineCount = 0;
	long file_stream;
	char line[1024];
	char separators[]=";\r\n";
	char* tmp_str;
	char *token[2][4];
	//open filestream
	if ((file_stream = fopen(filename, "r+")) == NULL) {
    	lr_error_message ("Cannot open %s", filename);
    	return -1;
    }
	i=0;
	// Read until end of file
    while (!feof(file_stream)) {
        // Read 1024 bytes per line 
        if (fgets(line, 1024, file_stream) != NULL)
         {
        	//Save lines count
        	LineCount++;
			j=0;        	
        	tmp_str = (char*)strtok(line, separators);
        	if (tmp_str!=NULL){
        		token[i][j]=(char*)malloc((strlen(tmp_str)+1)*sizeof(char));
				sprintf(token[i][j],"%s",tmp_str);
        	}
			    while (tmp_str!= NULL ) { 
        			j++;
			    	tmp_str = (char*)strtok(NULL, separators);
			    	if (tmp_str!=NULL){
			    		token[i][j]=(char*)malloc((strlen(tmp_str)+1)*sizeof(char));
			    		sprintf(token[i][j],"%s",tmp_str);
			    	}
			    }
        }
        i++;
    }

	//for (i=0;i<2;i++){for(j=0;j<4;j++){lr_message("element[%d][%d]=>%s",i,j,token[i][j]);}}

	if(token[0][0]!=NULL&&token[0][1]!=NULL&&token[0][2]!=NULL){
		InfluxDefine(token[0][0],token[0][1],token[0][2]);
		if(token[0][3]!=NULL){
			InfluxDefinePrecision(token[0][3]);
		}
		else{
			InfluxDefinePrecision("s");
		}
		if(token[1][0]!=NULL&&token[1][1]!=NULL){
			InfluxDefineAuth(token[1][0],token[1][1]);
		}
		else{
			InfluxDefineAuth("","");
		}
	}
	else{ 
		lr_error_message ("InfluxDefineFile %s FAILED", filename);
		ret=-1;
	}
	
	// Close the file stream 
    if (fclose(file_stream))
        lr_error_message ("Error closing file %s", filename);
    //free memory 
    for(i=0;i<2;i++){
    	for(j=0;j<4;j++){
    		free(token[i][j]);
    	}
    }
    
    return ret;	
}

void FAddTag(Param_map **head,char *tagname,char *paramname){
	int tmp_element_size;
	Param_map* tmp=(Param_map*) malloc(sizeof(Param_map));
	Param_map* counter=NULL;
	tmp->tag=(char*)malloc((strlen(tagname)+1)*sizeof(char));
	tmp->param=(char*)malloc((strlen(paramname)+1)*sizeof(char));
	sprintf(tmp->tag,"%s",tagname);
	sprintf(tmp->param,"%s",paramname);
	tmp->next=NULL;
	if ((*head)==NULL){
		(*head)=tmp;
	}
	else{
		counter=(*head);
		while (counter->next!=NULL){
			counter=counter->next;	
		}
		counter->next=tmp;
	}
	Param_map_array_length++;
	tmp_element_size=strlen(tagname)*3+1+strlen(paramname)*3+1;
	Param_map_array_size = Param_map_array_size + tmp_element_size;
	if(tmp_element_size>Param_map_array_max_element_size){
		Param_map_array_max_element_size = tmp_element_size;
	}
}

void FAddField(Field_map **head,char *P_Field,char *P_Param,char* P_FType){
	int tmp_element_size;
	Field_map* tmp=(Field_map*) malloc(sizeof(Field_map));
	Field_map* counter=NULL;
	tmp->Field=(char*)malloc((strlen(P_Field)+1)*sizeof(char));
	tmp->Param=(char*)malloc((strlen(P_Param)+1)*sizeof(char));
	tmp->FType=(char*)malloc((strlen(P_FType)+1)*sizeof(char));
	sprintf(tmp->Field,"%s",P_Field);
	sprintf(tmp->Param,"%s",P_Param);
	sprintf(tmp->FType,"%s",P_FType);
	tmp->next=NULL;
	if ((*head)==NULL){
		(*head)=tmp;
	}
	else{
		counter=(*head);
		while (counter->next!=NULL){
			counter=counter->next;	
		}
		counter->next=tmp;
	}
	Field_map_array_length++;
		tmp_element_size=strlen(P_Field)*3+1+strlen(P_Param)*3+1+strlen(P_FType)+1;
	Field_map_array_size = Field_map_array_size + tmp_element_size;
	if(tmp_element_size>Field_map_array_max_element_size){
		Field_map_array_max_element_size = tmp_element_size;
	}
}

void InfluxAddTag(char *tagname,char *paramname){
	FAddTag(&Param_map_array,tagname,paramname);
}

void InfluxAddField(char *P_Field,char *P_Param,char* P_FType){
	FAddField(&Field_map_array,P_Field,P_Param,P_FType);
}
	
int InfluxReadTag(Param_map *head,int pos,char** P_tag,char** P_param){	
	int i;
	Param_map* counter=NULL;
	//if array is empty 
	if (Param_map_array_length==0||pos>Param_map_array_length||head==NULL){
		return -1;
	}
	counter=head;
	for (i=0;i<pos&&counter->next!=NULL;i++){
			counter=counter->next;	
		}
	(*P_tag)=counter->tag;
	(*P_param)=counter->param;
	return 0;
	}


int InfluxReadField(Field_map *head,int pos,char** P_Field,char** P_Param,char** P_FType){	
	int i;
	Field_map* counter=NULL;
	//if array is empty 
	if (Field_map_array_length==0||pos>Field_map_array_length||head==NULL){
		return -1;
	}
	counter=head;
	for (i=0;i<pos&&counter->next!=NULL;i++){
			counter=counter->next;	
		}
	(*P_Field)=counter->Field;
	(*P_Param)=counter->Param;
	(*P_FType)=counter->FType;
	return 0;
	}

char* InfluxGetTagStr(char* P_Tag,char* P_Param){
	char *tmp_Tag,*tmp_Param;
	char* tmp_str=(char*)malloc(2*sizeof(char));
//--------------------------------------------------
	tmp_Tag=P_Tag;
	tmp_Param=lr_eval_string(P_Param);
	//Check if param exists
	if(strcmp(tmp_Param,P_Param)==0){
		//Parameter is not defined
		lr_error_message("Tag %s value is not defined %s",P_Tag,P_Param);
		tmp_str=(char*)realloc(tmp_str,2*sizeof(char));
		sprintf(tmp_str,"%s","");
	}
	else{
	//Assemble Tags string
		tmp_Tag=InfluxEscapeSymbols(tmp_Tag);
		tmp_Tag=InfluxEncodeString(tmp_Tag,NULL,LR_ENC_UTF8);
		tmp_Param=InfluxEscapeSymbols(tmp_Param);
		tmp_Param=InfluxEncodeString(tmp_Param,NULL,LR_ENC_UTF8);
		tmp_str=(char*)realloc(tmp_str,(strlen(tmp_Param)+strlen(tmp_Tag)+3)*sizeof(char));
		sprintf(tmp_str,",%s=%s",tmp_Tag,tmp_Param);
	}
	return tmp_str;
}

char* InfluxGetFieldsStr(char* P_Field,char* P_Param,char* P_FType){
	char *tmp_Field,*tmp_Param,*tmp_FType;
	char* tmp_srt=(char*)malloc(2*sizeof(char));
	//
	tmp_Field=P_Field;
	tmp_Param=lr_eval_string(P_Param);
	tmp_FType=P_FType;
	//Check if param exists
	if(strcmp(tmp_Param,P_Param)==0){
		//Parameter is not defined
		lr_error_message("Field %s value is not defined %s",P_Field,P_Param);
		tmp_srt=(char*)realloc(tmp_srt,2*sizeof(char));
		sprintf(tmp_srt,"%s","");
		return tmp_srt;
	}
	//Assemble field string
	if(strcmp(tmp_FType,"s")==0){
		tmp_Field=InfluxEscapeSymbols(tmp_Field);
		tmp_Field=InfluxEncodeString(tmp_Field,NULL,LR_ENC_UTF8);
		tmp_Param=InfluxEscapeSymbols(tmp_Param);
		tmp_Param=InfluxEncodeString(tmp_Param,NULL,LR_ENC_UTF8);
		tmp_srt=(char*)realloc(tmp_srt,(strlen(tmp_Field)+strlen(tmp_Param)+6+1)*sizeof(char));
		sprintf(tmp_srt,",%s=\"%s\"",tmp_Field,tmp_Param);
			}
	//Assemble field integer
	else if(strcmp(tmp_FType,"i")==0){
		if(atoi(tmp_Param)==0&&strcmp(tmp_Param,"0")!=0)
		{
			lr_error_message("Field %s value is not an integer",P_Field);
		}
		else{
		tmp_Field=InfluxEscapeSymbols(tmp_Field);
		tmp_Field=InfluxEncodeString(tmp_Field,NULL,LR_ENC_UTF8);
		tmp_srt=(char*)realloc(tmp_srt,(strlen(tmp_Field)+strlen(tmp_Param)+3+1)*sizeof(char));
		sprintf(tmp_srt,",%s=%si",tmp_Field,tmp_Param);	
		}
			}
	//Assemble field float
	else if(strcmp(P_FType,"f")==0){
			if(atof(tmp_Param)==0.0&&strcmp(tmp_Param,"0")!=0){
				lr_error_message("Field %s value is not a float",P_Field);
			}
			else{
				tmp_Field=InfluxEscapeSymbols(tmp_Field);
				tmp_Field=InfluxEncodeString(tmp_Field,NULL,LR_ENC_UTF8);
				tmp_srt=(char*)realloc(tmp_srt,(strlen(tmp_Field)+strlen(tmp_Param)+2+1)*sizeof(char));
				sprintf(tmp_srt,",%s=%s",tmp_Field,tmp_Param);
			}	
			}
	//Assemble field boolean
	else if(strcmp(tmp_FType,"b")==0){
			if(strcmp(tmp_Param,"true")==0||strcmp(tmp_Param,"false")==0){
				tmp_Field=InfluxEscapeSymbols(tmp_Field);
				tmp_Field=InfluxEncodeString(tmp_Field,NULL,LR_ENC_UTF8);
				tmp_srt=(char*)realloc(tmp_srt,(strlen(tmp_Field)+strlen(tmp_Param)+2+1)*sizeof(char));
				sprintf(tmp_srt,",%s=%s",tmp_Field,tmp_Param);
			}
			else{
				lr_error_message("Field %s value is not a boolean",P_Field);
			}
			}
	else{
		sprintf(tmp_srt,"%s","");
	}
	
	return tmp_srt;
}

void ReadALLTags (Param_map* P_Param_map_array,int P_Param_map_array_length){
	int i;
	char *tmp_tag,*tmp_param;
 	for(i=0;i<P_Param_map_array_length;i++){
		if (InfluxReadTag(P_Param_map_array,i,&tmp_tag,&tmp_param)==0){
		}
	}
}


void ReadALLFields (Field_map* P_Field_map_array,int P_Field_map_array_length){
	int i;
	char *tmp_field,*tmp_param,*tmp_type;
 	for(i=0;i<P_Field_map_array_length;i++){
		if (InfluxReadField(P_Field_map_array,i,&tmp_field,&tmp_param,&tmp_type)==0){
		}
	}
}

int InfluxAddTagFile (char* filename){
	int LineCount = 0; 
	long file_stream;
	char line[1024];
	char separators[]=";\r\n";
	char *token1="",*token2="";
	//open filestream
	if ((file_stream = fopen(filename, "r+")) == NULL) {
    	lr_error_message ("Cannot open %s", filename);
    	return -1;
    }
	
	// Read until end of file
    while (!feof(file_stream)) {
        // Read 1024 bytes per line 
        if (fgets(line, 1024, file_stream) != NULL)
         {
        	//Save lines count
        	LineCount++;         	
			    token1 = (char *)strtok(line, separators); // Get the first token (tag)
				token2 = (char *)strtok(NULL, separators); // Get second token (param)			    
			    if (!token1||!token2) {
			    }
				else{
				InfluxAddTag(token1,token2);
				}
        }
    }
		
	// Close the file stream 
    if (fclose(file_stream))
        lr_error_message ("Error closing file %s", filename);
    return 0;
}
	
	
	int InfluxAddFieldFile (char* filename){
	int LineCount = 0; 
	long file_stream;
	char line[1024];
	char separators[]=";\r\n";
	char *token1="",*token2="",*token3="";
	//open filestream
	if ((file_stream = fopen(filename, "r+")) == NULL) {
    	lr_error_message ("Cannot open %s", filename);
    	return -1;
    }
	
	// Read until end of file
    while (!feof(file_stream)) {
        // Read 1024 bytes per line 
        if (fgets(line, 1024, file_stream) != NULL)
         {
        	//Save lines count
        	LineCount++;         	
			    token1 = (char *)strtok(line, separators); // Get the first token (field)
				token2 = (char *)strtok(NULL, separators); // Get second token (param)
				token3 = (char *)strtok(NULL, separators); // Get second token (FType)
				
				if (!token3||(strcmp(token3,"s")!=0&&strcmp(token3,"f")!=0&&strcmp(token3,"i")!=0&&strcmp(token3,"b")!=0)) {
					token3 = "s";
			    	//Field type not recognized, using default (string);
			    }
			    if (!token1||!token2) {
			    }
				else{
				InfluxAddField(token1,token2,token3);
				}
        }
    }
	
	// Close the file stream 
    if (fclose(file_stream))
        lr_error_message ("Error closing file %s", filename);
    //ReadALLFields(Field_map_array,Field_map_array_length);
    return 0;
}

//fuction returns string for auth
char* InfluxGetAuthData(){
	char *tmp_str=(char*)malloc((strlen(Influx_User)+strlen(Influx_Password)+7)*sizeof(char));
	if (Is_influx_auth_enabled==1){
		sprintf(tmp_str,"&u=%s&p=%s",Influx_User,Influx_Password);
	}
	else{
		sprintf(tmp_str,"%s","");
	}
	return tmp_str;
}



// ----------------------------------------------------------------------------
//
// Description:
//    Get string of custom tegs and teg values(from LR parameters) 
//	  symbols ' ' '=' ',' escaped with '\' in tegs and teg vaues.
//
// Parameters:
//    P_Param_map_array (in)    - pointer to pointer to custom teg array
//    P_Param_map_array_length (in)    - number of pairs of elements in custom tag array 
//    
// Returns:
//    Returns a pointer to char.
//
// ----------------------------------------------------------------------------
char* InfluxGetCustomTagStr(Param_map* P_Param_map_array,const int P_Param_map_array_length){
	int i,tmp_size=1;
	char *tmp_str;
	char *res_str=(char*)malloc(2*sizeof(char));
	char *tmp_tag,*tmp_param;
	strcpy(res_str,"");
	for(i=0;i<P_Param_map_array_length;i++){
		InfluxReadTag(P_Param_map_array,i,&tmp_tag,&tmp_param);
		tmp_str=InfluxGetTagStr(tmp_tag,tmp_param);
		tmp_size=tmp_size+(strlen(tmp_str));
		if (i==0){
			res_str=(char*)realloc(res_str,(tmp_size)*sizeof(char));
			strcpy(res_str,tmp_str);
		}
		else{
			//lr_log_message("---%i res_str:%s  tmp_str:%s",tmp_size,res_str,tmp_str);
			res_str=(char*)realloc(res_str,(tmp_size)*sizeof(char));
			strcat(res_str,tmp_str);	
		}
		free(tmp_str);
		//lr_message("Tag__%s",res_str);
 	 	}
	return res_str;
}

char* InfluxGetCustomFieldsStr(Field_map* P_Field_map_array,const int P_Field_map_array_length){
	int i,tmp_size=1;
	char *tmp_str;
	char *res_str=(char*)malloc(2*sizeof(char));
	char *tmp_Field,*tmp_Param,*tmp_FType;
	strcpy(res_str,"");
	for(i=0;i<P_Field_map_array_length;i++){
		InfluxReadField(P_Field_map_array,i,&tmp_Field,&tmp_Param,&tmp_FType);
		tmp_str=InfluxGetFieldsStr(tmp_Field,tmp_Param,tmp_FType);
		tmp_size=tmp_size+(strlen(tmp_str));
		if (i==0){
			res_str=(char*)realloc(res_str,tmp_size*sizeof(char));
			strcpy(res_str,tmp_str);
		}
		else{
			res_str=(char*)realloc(res_str,tmp_size*sizeof(char));
			strcat(res_str,tmp_str);	
		}
		free(tmp_str);
		//lr_message("Field__%s",res_str);
 	 	}
	return res_str;
}


int OVERRIDED_lr_end_transaction (char* transation_name,int status) {
	int tmp_res,tr_status,tmp_vuser_id,tmp_scid,i;
	char *tmp_host,tmp_duration[100],*tmp_sfroup;
	double tr_duration,tr_thinktime,tr_wastedtime;
	char *tmp_tag,*tmp_param;
	char *tmp_str1;
	char *tmp_TName;
	
	//Check if influx is initialized	
	if (Is_influx_enabled==0){
		return lr_end_transaction(transation_name,status);
	}
	lr_start_transaction("Influx");
	
	//Save timestamp 
	//lr_save_timestamp("timestamp", "DIGITS=10", LAST);
	lr_save_string("","timestamp");
	
	//Get real transaction end status
	if (status==2) 
		tr_status=lr_get_transaction_status(transation_name);
		tr_status=status;
	
	//Get transaction duration, think_time, wasted_time
	tr_duration=1000*(lr_get_transaction_duration(transation_name)-lr_get_transaction_think_time(transation_name));
	tr_thinktime=1000*lr_get_transaction_think_time(transation_name);
	tr_wastedtime=1000*lr_get_transaction_wasted_time(transation_name);
	
	//End transaction using standart LR function
	tmp_res=lr_end_transaction(transation_name,status);
	
	//get generator host name
	tmp_host=lr_get_host_name();
	
	//Get Vusr id
	lr_whoami(&tmp_vuser_id,&tmp_sfroup,&tmp_scid);
	//prepare trnsaction name
	tmp_TName=InfluxEscapeSymbols(transation_name);
	tmp_TName=InfluxEncodeString(tmp_TName,NULL,LR_ENC_UTF8);
	
	//prepare standart tags and vals
	lr_save_string ("Duration","val1");				lr_save_int(tr_duration,"val1_val"); 
	lr_save_string ("Think_time","val2");			lr_save_int(tr_thinktime,"val2_val"); 
	lr_save_string ("Wasted_time","val3");			lr_save_int(tr_wastedtime,"val3_val"); 
	lr_save_string ("Vuser_ID","val4"); 			lr_save_int (tmp_vuser_id,"val4_val"); 
	lr_save_string ("Transaction_name","tag1");  	lr_save_string(tmp_TName,"tag1_val");
	lr_save_string ("Transaction_status","tag2"); 	lr_save_int (tr_status,"tag2_val");
 	lr_save_string ("Load_generator","tag3"); 		lr_save_string (tmp_host,"tag3_val");
 	
 	//prepare influx prefs
 	lr_save_string (Influx_URL,"Influx_URL");
 	lr_save_string (Influx_DB,"Influx_DB");
 	lr_save_string (Influx_Measurement,"Influx_Measurement");
 	lr_save_string (Influx_Precision,"Influx_precision");
 	
 	lr_save_string (InfluxGetAuthData(),"InfluxAuthData");
 	
 	//prepare custom Tags string part
 	tmp_tag=InfluxGetCustomTagStr(Param_map_array,Param_map_array_length);
 	lr_save_string(tmp_tag,"custom_tags");
 	free(tmp_tag);
 	//prepare custom Fields string part
 	tmp_param=InfluxGetCustomFieldsStr(Field_map_array,Field_map_array_length);
 	lr_save_string(tmp_param,"Custom_Fields"); 
 	free(tmp_param);
 	
 	//Info messages
 	//lr_message("Influx url++>%s",lr_eval_string("URL={Influx_URL}/write?db={Influx_DB}&precision={Influx_precision}{InfluxAuthData}"));
 	//lr_message("Influx line++>%s",lr_eval_string("Body={Influx_Measurement},{tag1}={tag1_val},{tag2}={tag2_val},{tag3}={tag3_val}{custom_tags} {val1}={val1_val}i,{val2}={val2_val}i,{val3}={val3_val}i,{val4}={val4_val}i{Custom_Fields} {timestamp}"));
 	
 	//Write data into InfluxDB
	
	web_custom_request("try_write", 						
		"Method=POST",
		lr_eval_string("URL={Influx_URL}/write?db={Influx_DB}&precision={Influx_precision}{InfluxAuthData}"),
		lr_eval_string("Body={Influx_Measurement},{tag1}={tag1_val},{tag2}={tag2_val},{tag3}={tag3_val}{custom_tags} {val1}={val1_val}i,{val2}={val2_val}i,{val3}={val3_val}i,{val4}={val4_val}i{Custom_Fields} {timestamp}"),
		LAST);	
	lr_end_transaction("Influx", LR_AUTO);
	return tmp_res;
}

//Override lr_end_transaction
#define lr_end_transaction(transaction_name,status) OVERRIDED_lr_end_transaction(transaction_name,status)