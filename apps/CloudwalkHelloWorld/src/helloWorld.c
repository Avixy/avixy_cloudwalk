#include "core/device_core.h"
#include "core/trace.h"
#include "avixy/gfx.h"

#include <fcntl.h>
#include <sys/stat.h>

/* Include the mruby header */
#include "mruby.h"
#include "mruby/value.h"
#include "mruby/compile.h"
#include "mruby/proc.h"

// Vamos verificar se existe arquivo indicando que estamos num ambiente de desenvolvimento com NFS.
// Caso contrario as mensagens ser�o direcionadas para a "saida padr�o" que est� associada a porta serial do equipamento.
// Velocidade 115200, 8 bits, sem paridade e sem controle de fluxo
void inicializaTrace(void)
{	int fdTrace=1;
	struct stat stat_buf;
	const char *fileName="traceFileNFS.txt";

	if ( (fileName != NULL) && (stat(fileName, &stat_buf) == 0 ) ) {
		if ( S_ISREG(stat_buf.st_mode) )  {
			if ( (fdTrace = open(fileName, O_WRONLY | O_APPEND |  O_SYNC , 0666)) > 0 ) {
				// Como estamos em modo de desenvolvimento NFS, vamos programar a tecla F2 para abortar o programa (ao recarregar j� vai pegar a nova vers�o do programa recm compilado).
				kbdSetExitAppKey(KBD_F2);
			}
		}
	}

	traceInit(fdTrace);
	traceBaseLocaltimeClock();	// opcionalmente poderiamos utilizar traceBasetimeZero()

	traceStart(TRACE_APP1);
	traceStart(TRACE_DESENV);

	tracePrint(TRACE_APP1, " ");
	tracePrint(TRACE_APP1, " ");
	tracePrint(TRACE_APP1, "%s: TRACE_APP1 Habilitado. Serial: %u Compilado em: %s as %s", __FUNCTION__, avxGetSerialNumber(), __DATE__, __TIME__);
}

int _pp_print_debug(char const* arg, ...){
	return 0;
}

int _pp_print_error(char const* arg, ...){
	return 0;
}

// Define um vetor de cores para serem utilizadas durante a apresenta��o
int vetCores[] = {white, black, red, green, dark_green, yellow, blue};
#define MAX_CORES   7


//--------------------------------------------------------------------------------------------------------
// Programa principal
//--------------------------------------------------------------------------------------------------------
int main()
{
	bitmap_t *bitmapBackground;

	avxCoreInit();					// Inicializa��o dos recursos necess�rios para o equipamento funcionar adequadamente. OBRIGAT�RIO.
	avxSetBacklight(-1, -1);		// Controle da intensidade de ilumina��o do teclado e do display autom�tico
	inicializaTrace();
	lcdInit(ALTURA_STATUS_BAR);

	// Carrega arquivo ".bmp" para a regi�o de mem�ria definida anteriormente e seta como "tela de fundo"
	bitmapBackground = bitmapLoadFromFile("/app/imagens/TelaFundo.bmp", NULL);

	if ( bitmapBackground ) {
		lcdSetBackgroundBmp(bitmapBackground);
		lcdSetTransparency(LCD_TRANSPARENCY_BITMAP);
	}

	lcdSetFont(LCD_FONT_STANDARD, LCD_FONT_CONTINUOUS);

	// Cria uma janela simples, s� com �rea CLIENT (sem �rea de t�tulo, sem �rea de scroll, etc) com o  maior tamanho poss�vel.
	lcdCreateWindow(LCD_MASK_CLIENT,999,999, "");

	while ( 1 )
	{
		mrb_state *mrb;
		mrbc_context *c;
		char code[] = "Context.start(\"main\", \"Platform\")";

		mrb = mrb_open();
		c   = mrbc_context_new(mrb);

		mrb_load_string_cxt(mrb, code, c);
		mrbc_context_free(mrb, c);

		mrb_close(mrb);

		kbdWaitKey(5000);
	}

	return 0;
}
