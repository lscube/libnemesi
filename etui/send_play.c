/* * 
 *  ./etui/send_play.c: $Revision: 1.2 $ -- $Date: 2002/11/07 12:12:10 $
 *  
 *  This file is part of NeMeSI
 *
 *  NeMeSI -- NEtwork MEdia Streamer I
 *
 *  Copyright (C) 2001 by
 *  	
 *  	Giampaolo "mancho" Mancini - manchoz@inwind.it
 *	Francesco "shawill" Varano - shawill@infinto.it
 *
 *  NeMeSI is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  NeMeSI is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with NeMeSI; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *  
 * */

#include <nemesi/etui.h>

int send_play(struct RTSP_args *rtsp_args, char *argstr)
{
	char *tkn;
	char *meno;
	char start[9]="", stop[9]="";
	char flag=0;
	char zz[2][3][2]={{"00", "00", "00"},{"00", "00", "00"}};
	int i, j;
	
		
	*(argstr+strlen(argstr)-1)='\0';
	while(*++argstr == ' ');

	meno=argstr;
	j=0;
	
	if(*argstr){
			
		for(tkn=argstr; tkn && (isdigit(*tkn) || (*tkn == ':')  || (*tkn == '-') || isspace(*tkn)); tkn++);
		if (*tkn){
			uiprintf("Mi shfiti? A che mi shfiti? A tesht' de fero?\n");
			return 1;
		}
		
		if((tkn=strchr(argstr, '-')) != NULL){
			*tkn='\0';
			if (strlen(++tkn)){
				flag=1;
				j=1;
				meno=tkn;
			}
		}
		while (j >= 0 ){
			for(i=2; i>=0; i--){
				if((tkn=strrchr(meno, ':')) != NULL){
					if(strlen(tkn+1) > 2){
						uiprintf("Mi shfiti? A che mi shfiti? A piet' de fero?\n");
						return 1;
					}
					if(strlen(tkn+1) < 2)
						zz[j][i][1]=*(tkn+1);
					else { 
						zz[j][i][0]=*(tkn+1);
						zz[j][i][1]=*(tkn+2);
					}
					*tkn='\0';
				} else {
					if(strlen(meno) > 2){
						uiprintf("Mi shfiti? A che mi shfiti? A piet' de fero?\n");
						return 1;
					}
					if(strlen(meno) < 2)
						zz[j][i][1]=*(meno);
					else { 
						zz[j][i][0]=*(meno);
						zz[j][i][1]=*(meno+1);
					}
					break;
				}
			}
			if (*zz[j][2] > '5' || *zz[j][1] > '5' || (*zz[j][0] > '1' && *(zz[j][0]+1) > '4') ){
				uiprintf("Mi shfiti? A che mi shfiti? A panz' de fero?\n");
				return 1;
			}
			j--;
			meno=argstr;
		}
		sprintf(start, "%c%c:%c%c:%c%c", zz[0][0][0], zz[0][0][1], zz[0][1][0], zz[0][1][1], zz[0][2][0], zz[0][2][1]);
		if (flag){
			sprintf(stop, "%c%c:%c%c:%c%c", zz[1][0][0], zz[1][0][1], zz[1][1][0], zz[1][1][1], zz[1][2][0], zz[1][2][1]);
		
			if ( strcmp(start, stop) >= 0){
				uiprintf("Mi shfiti? A che mi shfiti? A mano de fero?\n");
				return 1;
			}
		}
	}

	pthread_mutex_lock(&(rtsp_args->comm_mutex));
		rtsp_args->comm->opcode= PLAY;
		if (*argstr){
			sprintf(rtsp_args->comm->arg, "%s-", start);
			if (flag)
				sprintf(rtsp_args->comm->arg+strlen(rtsp_args->comm->arg), "%s", stop);
		} else
			*(rtsp_args->comm->arg)='\0';
		write(rtsp_args->pipefd[1], "p", 1);
		rtsp_args->rtsp_th->busy=1;
	pthread_mutex_unlock(&(rtsp_args->comm_mutex));

	return 0;
}
