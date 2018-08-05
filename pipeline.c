#include <ctype.h>
#include <unistd.h>

#include "function.c"

int main(int argc, char **argv){
	int c = 0;
    int h = 0;
	int u = 0;
	int n = 0;
	int b = 0;
	int index;
	int c1;
	int opterr = 0;

    pthread_t *threads;

	while((c1 =  getopt(argc, argv,"c:h:u:n:b")) != -1){
		switch(c1){
			case 'c':
				if(atof(optarg) <= 0){
					printf("El valor ingresado para la cantidad de imagenes es invalido.");
					c1 = -1;
					break;
				}
				else c = atof(optarg);
				break;
            case 'h':
                if(atof(optarg) < 0){
                    printf("El valor ingresado para iniciar las hebras es invalido.");
                    c1 = -1;
                    break;
                }
                else{
                    h = atof(optarg);
                    threads = malloc(h * sizeof(pthread_t));
                } 
                break;
            case 'u':
				if(atof(optarg) < 0 && atof(optarg) >= 255){
					printf("El valor ingresado para el umbral es invalido. El valor a ingresar debe estar\nentre 0 y 255");
					c1 = -1;
					break;
				}
				else u = atof(optarg);
				break;
			case 'n':
				if(atof(optarg) < 0 && atof(optarg) > 100){
					printf("El valor ingresado para la clasificacion es invalido. Ingrese un numero entre 0 y 100");
					c1 = -1;	
					break;
				}
				else n = atof(optarg);
				break;
			case 'b':
				b = 1;
				break;	
			case '?':
				if(optopt == 'c' || optopt == 'u' || optopt == 'n')
					fprintf(stderr, "Option -%c requeries an argument.\n",optopt);
				else if(isprint(optopt))
					fprintf(stderr,"Unknown option -%c.\n",optopt);
				else
					fprintf(stderr, "Unknown option character `\\x%x'.\n",optopt);
				return 1;
			default:
				abort();
		}

		//for(index = optind; index < argc; index++)
		//	printf("Non-option argument %s\n",argv[index]);
	}
	if(b == 1){
		printf("\n *************************************\n");
		printf("|   image          |   nearly black   |\n");
		printf("|------------------|------------------|\n");	
	}
	else
		printf("Ha ingresado no mostrar conclusion del analisis.\n");
	execute_task(c,h,threads,u,n,b);
	printf(" *************************************\n\n");
	return 0;

}