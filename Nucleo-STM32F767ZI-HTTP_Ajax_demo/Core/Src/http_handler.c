#include <http_handler.h>
#include "string.h"
#include "stdio.h"
#include "lwip/tcp.h"
#include "lwip/apps/httpd.h"
#include "stm32f7xx_hal.h"
#include "stdbool.h"
#include "stdio.h"

#include "math.h"

#define NUM_OF_CGIS		1
#define NUM_OF_TAGS 	4

bool LedNum1, LedNum2;
char const * ssi_tags[]= {"tag1_ucT","tag2_Ld1","tag3_Ld2","tag_ajax"};
char const ** tags = ssi_tags;
const char * led_cgi_handler (int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);

const tCGI LED_CGI ={"/leds.cgi",led_cgi_handler};
tCGI CGI_ARR[NUM_OF_CGIS];


const char * led_cgi_handler (int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
	bool ActualLed1State =false;
	bool ActualLed2State = false;
	uint32_t i = 0;

	if( iIndex == 0)
	{
		for( i = 0; i < iNumParams; i++)
		{
			if(strcmp(pcParam[i],"led") == 0)
			{
				if(strcmp(pcValue[i],"1") == 0)
				{
					ActualLed1State = true;
				}
				else if(strcmp(pcValue[i],"2") == 0)
				{
					ActualLed2State = true;
				}
			}
		  }
	}

	if(ActualLed1State == true && LedNum1 == false){
		LedNum1 = true;
		HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_SET);
	}
	if(ActualLed1State == false && LedNum1 == true){
		LedNum1 = false;
		HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_RESET);
	}

	if(ActualLed2State == true && LedNum2 == false){
		LedNum2 = true;
		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
	}
	if(ActualLed2State == false && LedNum2 == true){
		LedNum2 = false;
		HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_RESET);
	}

	return "/index.shtml";
}


uint16_t  ssi_handler(int iIndex,char *pcInsert, int iInsertLen)
{
	static float Angle = 0.0F;

	switch(iIndex){
		case 0:
			sprintf(pcInsert, "%d",(int)HAL_GetTick() );
			return strlen(pcInsert);
			break;

		case 1:
			if(LedNum1 == true)
			{
				sprintf(pcInsert,"<input value=\"1\" name=\"led\" class=\"largerCheckbox\" type=\"checkbox\" checked>");
				return strlen(pcInsert);
			}
			else if(LedNum1 == false)
			{
				sprintf(pcInsert,"<input value=\"1\" name=\"led\" class=\"largerCheckbox\" type=\"checkbox\">");
				return strlen(pcInsert);
			}
			break;
		case 2:
			if(LedNum2 == true)
			{
				sprintf(pcInsert,"<input value=\"2\" name=\"led\" class=\"largerCheckbox\" type=\"checkbox\" checked>");
				return strlen(pcInsert);
			}
			else if(LedNum2 == false)
			{
				sprintf(pcInsert,"<input value=\"2\" name=\"led\" class=\"largerCheckbox\" type=\"checkbox\">");
				return strlen(pcInsert);
			}
			return strlen(pcInsert);
			break;
		case 3:
			sprintf(pcInsert, ",%f,%f,%d",sin(Angle),cos(Angle),(int)HAL_GetTick());
			Angle = Angle + 0.3F;
			return strlen(pcInsert);

			break;
		default:
			break;

	}
	return 0;
}


void http_server_init (void)
{
	httpd_init();

	http_set_ssi_handler(ssi_handler, (char const**) tags, NUM_OF_TAGS);

	CGI_ARR[0] = LED_CGI;

	http_set_cgi_handlers (CGI_ARR, NUM_OF_CGIS);
}
