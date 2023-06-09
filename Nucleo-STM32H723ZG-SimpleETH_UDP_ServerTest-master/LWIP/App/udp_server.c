#include <lwip/udp.h>
#include <udp_server.h>
#include "gpio.h"

#include "lwip/prot/dhcp.h"
#include "lwip/dhcp.h"
#include "lwip.h"


//to zostawie bo ciekawe -- gwarantuje ze pola struktury beda w adresach po sobie...
//tak jak logika podpowiada :D
PACK_STRUCT_BEGIN
struct pdu_t {
  PACK_STRUCT_FIELD(u8_t version_operation);
  PACK_STRUCT_FIELD(u8_t port);
  PACK_STRUCT_FIELD(u16_t data);
} PACK_STRUCT_STRUCT;
PACK_STRUCT_END

void recv_callback (void *arg, struct udp_pcb *pcb, struct pbuf *p,
    const ip_addr_t *addr, u16_t port);

/* Tylko ta funkcja nie jest wołana w kontekście procedury obsługi
   przerwania. */

extern struct netif gnetif;


int UDPserverStart(uint16_t port) {


	 uint32_t Timeoutdhcp=HAL_GetTick();
		  struct dhcp *dhcp;
		  dhcp = netif_dhcp_data(&gnetif);
		  while(dhcp->state != DHCP_STATE_BOUND ) //blokujaco do oporu az sie polaczy... XD
		  {
			  MX_LWIP_Process ();  //wiadomo libka musi dzialac wczesniej se ja zblokowalem
			  	  	  	  	  //dobrze ze sie szybko kapnolem
			  	if( (HAL_GetTick()-Timeoutdhcp) > 10000 )  //timeout 10sek
			  	{
			  		break;
			  	}
		  }			//Ja nie wiem czego ST tego nie umieszcza np w przerwaniu od systicka :/
		  	  	  	  	  	  //no Najlepiej to RTOSik odzielny task i po zawodach :) i tak w RTOSie moze to robia nwm nie sprawdzalem

  struct udp_pcb *pcb;
  err_t err;

  pcb = udp_new();

  if (pcb == NULL)
    return -1;

  /* Ostatni parametr ma wartość NULL, bo serwer jest bezstanowy. */
  udp_recv(pcb,(void*)recv_callback, NULL);


  err = udp_bind(pcb, IP_ADDR_ANY, port);

  if (err != ERR_OK) {

    udp_remove(pcb);

    return -1;
  }
  return 0;
}

void recv_callback(void *arg, struct udp_pcb *pcb, struct pbuf *p,
    const ip_addr_t *addr, u16_t port)
	{
  struct pbuf *q;
 // struct pdu_t *src, *dst;

  // Potrzebny jest bufor do wysłania odpowiedzi. Nie jest jasne,
   //  dlaczego próba użycia w tym celu p nie działa.
  q = pbuf_alloc(PBUF_TRANSPORT, sizeof(struct pdu_t), PBUF_RAM);
  if (q == NULL) {
    pbuf_free(p);
    return;
  }

//  src = p->payload;
//  dst = q->payload;
  q->payload="Pozdrawiam Dziala! \n\r";
  q->len=sizeof("Pozdrawiam Dziala! \n\r");
  q->tot_len=q->len;

  pbuf_free(p);
  udp_sendto(pcb, q, addr, port);
  pbuf_free(q);
}

