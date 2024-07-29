#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include "IO.c"

#undef NULL
#define NULL (void *)0

#define MAXMEM 4096
#define START 0x200

#define FI (0xf000 & inst) >> 12
#define SE (0xf00 & inst) >> 8
#define TH (0xf0 & inst) >> 4
#define FO (0xf & inst)

#define NNN 0xfff & inst
#define KK 0xff & inst
//#define DEBUGINP 1

static uint8_t Mem[MAXMEM] = 
	{
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};

#define NUMREG 16
static uint8_t V[NUMREG] = {0} ;

uint16_t I  = 0 ;

uint8_t delay = 0 , sound = 0 ;

uint16_t PC = 0 ;

#define STKRGE 16
uint16_t Stack[STKRGE] ;
uint8_t SP = 0 ;

void load(char *name)
{
	FILE *inp = fopen(name , "rb") ;
	if(inp == NULL)
	{
		printf("Your file ain't worthy \n") ;
		goto CLOSE ;
	}

	fseek(inp , 0 , SEEK_END) ;
	uint64_t FSize = ftell(inp) ;
	rewind(inp) ;
	fread(&Mem[START] , 1 , FSize , inp) ;
	PC = START ;

#ifdef DEBUGINP
	for(int i = START ; i < MAXMEM ; i++)
	{
		printf("instruction %x : %X \t" , i , Mem[i] ) ;
		if(!(i % 8)) printf("\n") ;
	}
#endif

	fclose(inp) ;

CLOSE:
	return ;
}

void SYS()
{
	printf("SYS CALLL \n") ;
	//this inst is ignored by compilers
	PC += 2 ; 
}

void CLS()
{
	for(int x = 0 ; x < WRES ; x++)
		for(int y = 0 ; y < HRES ; y++)
			gfx[x][y] = 0 ;
	draw = 1 ;
	PC += 2 ;
}

void RET()
{
	PC = Stack[SP] ;
	SP-- ;
	PC += 2 ;
}

void JP(uint16_t inst)
{
	PC = NNN ;
}

void CALL(uint16_t inst)
{
	SP++ ;
	Stack[SP] = PC ;
	PC = NNN ;
}

void SEval(uint16_t inst)
{
	if(V[SE] == (KK))
		PC += 2 ;
	PC += 2 ;
}

void SNEval(uint16_t inst)
{
	if(V[SE] != (KK))
		PC += 2 ;
	PC += 2 ;
}

void SEreg(uint16_t inst)
{
	if(V[SE] == V[TH])
		PC += 2 ;
	PC += 2 ;
}

void LDval(uint16_t inst)
{
	V[SE] = KK ;
	PC += 2 ;
}

void ADDval(uint16_t inst)
{
	V[SE] += KK ;
	PC += 2 ;
}

void LDreg(uint16_t inst)
{
	V[SE] = V[TH] ;
	PC += 2 ;
}

void OR(uint16_t inst)
{
	V[SE] = V[SE] | V[TH] ;
	PC += 2 ;
}

void AND(uint16_t inst)
{
	V[SE] = V[SE] & V[TH] ;
	PC += 2 ;
}

void XOR(uint16_t inst)
{
	V[SE] = V[SE] ^ V[TH] ;
	PC += 2 ;
}

void ADDreg(uint16_t inst)
{
	uint8_t tmp = V[SE] ;
	tmp += V[TH] ;
	if((tmp < V[SE]) && (tmp < V[TH]))
		V[0xf] = 1 ;
	PC += 2 ;
}

void SUB(uint16_t inst)
{
	if(V[SE] > V[TH])
		V[0xf] = 1 ;
	V[SE] -= V[TH] ;
	PC += 2 ;
}

void SHR(uint16_t inst)
{
	V[0xf] = V[SE] & 0x1 ;
	V[SE] = V[SE] / 2 ;
	PC += 2 ;
}

void SUBN(uint16_t inst)
{
	if(V[TH] > V[SE])
		V[0xf] = 1 ;
	V[SE] = V[TH] - V[SE] ;
	PC += 2 ;
}

void SHL(uint16_t inst)
{
	V[0xf] = (V[SE] & 0x80)/128 ;
	V[SE] = V[SE] * 2 ;
	PC += 2 ;
}

void SNEreg(uint16_t inst)
{
	if(V[SE] != V[TH])
		PC += 2 ;
	PC += 2 ;
}

void LDaddr(uint16_t inst)
{
	I = NNN ;
	PC += 2 ;
}

void JPreg(uint16_t inst)
{
	PC = NNN + V[0] ;
}

void RND(uint16_t inst)
{
	time_t t ;
	srand((unsigned) time(&t)) ;
	V[SE] = (rand() % 255) & KK ;
	PC += 2 ;
}

void DRW(uint16_t inst)
{
	V[0xf] = 0 ;
	for(int y = 0 ; y < FO ; y++)
		for(int x = 0 ; x < 8 ; x++)
		{
			if(Mem[I + y] & (0x80 >> x))
			{
				if(gfx[(V[SE] + x)%WRES][(V[TH] + y)%HRES])
					V[0xf] = 1 ;

				gfx[(V[SE] + x)%WRES][(V[TH] + y)%HRES] ^= 1 ;

			}
		}
	draw = 1 ;
	PC += 2 ;
}

void SKP(uint16_t inst)
{
	if(key == V[SE])
	{
		key = 0 ;
		PC += 2 ;
	}
	PC += 2 ;
}

void SKPN(uint16_t inst)
{
	if(key != V[SE])
		PC += 2 ;
	key = 0 ;
	PC += 2 ;
}

void LDdtime(uint16_t inst)
{
	V[SE] = delay ;
	PC += 2 ;
}

void LDkey(uint16_t inst)
{
		if(key)
		{
			V[SE] = key ;
			key = 0 ;
			PC += 2 ;
			return ;
		}

}

void LDsetdelay(uint16_t inst)
{
	delay = V[SE] ;
	PC += 2 ;
}

void LDsound(uint16_t inst)
{
	sound = V[SE] ;
	PC += 2 ;
}

void ADDi(uint16_t inst)
{
	I += V[SE] ;
	PC += 2 ;
}

void LDsprite(uint16_t inst)
{
	I = V[SE]*5 ;
	PC += 2 ;
}

void LDbcd(uint16_t inst)
{
	Mem[I + 2] = V[SE]%10 ;
	Mem[I + 1] = (V[SE]/10) % 10 ;
	Mem[I] = (V[SE]/100) % 10 ;
	PC += 2 ;
}

void LDstorereg(uint16_t inst)
{
	for(int i = 0 ; i <= SE ; i++)
		Mem[I + i] = V[i] ;
	PC += 2 ;
}

void LDrestorereg(uint16_t inst)
{
	for(int i = 0 ; i <= SE ; i++)
		V[i] = Mem[I + i] ;
	PC += 2 ;
}

int main(int argc , char **argv)
{
	atexit(cleanup) ;
	InitSDL() ;
	SDL_Event event ;
	if(argc < 3)
	{
		printf("Supply File with -f \n") ;
		exit(0) ;
	}

	load(argv[2]) ;

	uint16_t opcode = 0 ;
	while(1)
	{
		if(key)
			printf("KEY val : %d \n" , key) ;
		//SDL_Delay(16) ;
		Inp(event) ;
		opcode = Mem[PC] ;
		opcode <<= 8 ;
		opcode += Mem[PC + 1] ;
		printf("WORKING %x \n" , opcode) ;

		if(draw)
		{
			Draw(gfx) ;
			draw = 0 ;
		}

		switch(opcode & 0xF000) {

			case 0x0000:
				switch(opcode & 0x00FF) {
					case 0x00E0:
						CLS() ;
						break;

					case 0X00EE:
						RET() ;
						break ;

					default:
						printf("Unknown in 0x00 %d \n" , opcode);
						break;
				}
			break;

			case 0x1000:
				JP(opcode) ;
				break;

			case 0x2000:
				CALL(opcode) ;
				break;

			case 0x3000:
				SEval(opcode) ;
				break;

			case 0x4000:
				SNEval(opcode) ;
				break;

			case 0x5000:
				SEreg(opcode) ;
				break;

			case 0x6000:
				LDval(opcode) ;
				break;

			case 0x7000:
				ADDval(opcode) ;
				break ;
			case 0x8000:
				switch(opcode & 0x000F) {
					case 0x0000:
						LDreg(opcode) ;
						break;

					case 0x0001:
						OR(opcode) ;
						break;

					case 0x0002:
						AND(opcode) ;
						break;

					case 0x0003:
						XOR(opcode) ;
						break;

					case 0x0004:
						ADDreg(opcode) ;
						break;

					case 0x0005:
						SUB(opcode) ;
						break;

					case 0x0006:
						SHR(opcode) ;
						break;

					case 0x0007:
						SUBN(opcode) ;
						break;

					case 0x000E:
						SHL(opcode) ;
						break;

					default: {
						printf("Unknown opcode\n");

					}
					break;
				}

			break;

			case 0x9000:
			SNEreg(opcode) ;
			break;

			case 0xA000:
			LDaddr(opcode) ;
			break;

			case 0xB000:
			JPreg(opcode) ;
			break;

			case 0xC000:
			RND(opcode) ;
			break;

			case 0xD000:
			DRW(opcode) ;
			break;

			case 0xE000:
				switch(opcode & 0x00FF)
				{
					case 0x009E:
						SKP(opcode) ;
						break;

					case 0x00A1 :
						SKPN(opcode) ;
						break;

					default: {
						printf("Unknown at 0xE\n");

					}
					break;
				}
			break;


			case 0xF000:
				switch(opcode & 0x00FF) {
					case 0x0007:
						LDdtime(opcode) ;
						break;

					case 0x000A:
						LDkey(opcode) ;
						break;

					case 0x0015:
						LDsetdelay(opcode) ;
						break;

					case 0x0018:
						LDsound(opcode) ;
						break;

					case 0x001E:
						ADDi(opcode) ;
						break;

					case 0x0029:
						LDsprite(opcode) ;
						break;

					case 0x0033:
						LDbcd(opcode) ;
						break;

					case 0x0055:
						LDstorereg(opcode) ;
						break;

					case 0x0065:
						LDrestorereg(opcode) ;
						break;

					default: {
						printf("Unknown opcode\n");

					}
					break;
				}
			
			break;

			default:
				printf("Unknown opcode\n");
				break;
		}
	
		if(delay > 0)
			delay--;

		if(sound > 0)
			sound -- ;

	}
	return 0 ;

}

