#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#define MAX_BUFFER 1024
#define ss95el1_ADRES_READ "/proc/sykom/ss95el1"
#define ss95el2_ADRES_WRITE "/proc/sykom/ss95el2"
#define ss95el3_ADRES_READ "/proc/sykom/ss95el3"
#define ss95el4_ADRES_WRITE "/proc/sykom/ss95el4"
#define ss95el5_ADRES_READ "/proc/sykom/ss95el5"
#define ss95el6_ADRES_WRITE "/proc/sykom/ss95el6"



#define GPIO_1_TEST_VALUE_1         0x6000
#define GPIO_1_TEST_VALUE_2         0x7000
#define GPIO_1_TEST_VALUE_3         0x240
#define GPIO_2_TEST_VALUE_1         0x6000000
#define GPIO_2_TEST_VALUE_2         0x7000000
#define GPIO_2_TEST_VALUE_3         0x140000

//void test_int(){
//  char buffer[MAX_BUFFER];
//  printf("Test INT\n");
//  int i = 0;
//  while(i < 45){
//    int ss95el5_adres_read=open(ss95el5_ADRES_READ, O_RDONLY); //otwarcie pliku „out” sterownika
//    if(ss95el5_adres_read<0){
//      printf("Open %s – error: %d\n", ss95el5_ADRES_READ, errno);
//      exit(6);
//    }
//    
//    int ss95el5_adres_read_odczyt =read(ss95el5_adres_read, buffer, MAX_BUFFER); //odczyt danych ze sterownika
//   
//    if(ss95el5_adres_read_odczyt>0){
//      buffer[ss95el5_adres_read_odczyt]='\0';
//      printf("INT %s\n", buffer);
//      }
//  
//    sleep(1);
//    i++;
//    close(ss95el5_adres_read);
//  
//    }
//  }



void read_from_file(char *filePath){
	char buffer[MAX_BUFFER];
	int fd_out=open(filePath, O_RDONLY);
	if(fd_out<0){
		 printf("Open %s - error %d\n", filePath, errno);
		 exit(1);
	}
	int n = read(fd_out, buffer, MAX_BUFFER);
    if(n>0){
        buffer[n]='\0';
        printf("%s", buffer); 
    }else{
        printf("Open %s - error %d\n", filePath, errno); 
    }
	close(fd_out); 
}




void write_to_file(char *filePath, unsigned int input){
	char buffer[MAX_BUFFER];
	int fd_in=open(filePath, O_RDWR); 
	if(fd_in < 0){
		 printf("Open %s - error number %d\n", filePath, errno);
		 exit(2);
	}
	snprintf(buffer, MAX_BUFFER, "%x", input);
	int n=write(fd_in, buffer, strlen(buffer));
    if(n!=strlen(buffer)){
        printf("Open %s - error number %d\n", filePath, errno);
        close(fd_in);
        exit(3);
    }
	close(fd_in);
}



void gpio_read_test(){
    printf("Odczyt z czujnika 1: ");
    read_from_file(ss95el1_ADRES_READ);
    printf("Odczyt z czujnika 2: ");
    read_from_file(ss95el3_ADRES_READ);
}


void gpio_write_test(){
    write_to_file(ss95el2_ADRES_WRITE, GPIO_1_TEST_VALUE_1);
    write_to_file(ss95el2_ADRES_WRITE, GPIO_1_TEST_VALUE_2);
    write_to_file(ss95el4_ADRES_WRITE, GPIO_2_TEST_VALUE_1);
    // write_to_file(ss95el4_adres_write, GPIO_1_TEST_VALUE_3);
    write_to_file(ss95el4_ADRES_WRITE, GPIO_2_TEST_VALUE_2);
    // write_to_file(ss95el4_adres_write, GPIO_2_TEST_VALUE_3);
}


void test_int(){
  char buffer[MAX_BUFFER];
  printf("Testowanie INTa\n");
  int i = 0;
  while(i < 30){
    int ss95el5_adres_read=open(ss95el5_ADRES_READ, O_RDONLY); //otwarcie pliku „out” sterownika
    if(ss95el5_adres_read<0){
      printf("Open %s – error: %d\n", ss95el5_ADRES_READ, errno);
      exit(6);
    }
    
    int ss95el5_adres_read_odczyt =read(ss95el5_adres_read, buffer, MAX_BUFFER); //odczyt danych ze sterownika
   
    if(ss95el5_adres_read_odczyt>0){
      buffer[ss95el5_adres_read_odczyt]='\0';
      printf("wartosc int %s\n", buffer);
      }
  
    sleep(1);
    i++;
    close(ss95el5_adres_read);
    }
  }

int main(void){
//  
//  char buffer[MAX_BUFFER];
//  test_int();
//  
//
//  printf("Zapis jedynki logicznej z przesunięciem 7 bitów na INT\n");
//  int ss95el6_adres_write=open(ss95el6_ADRES_WRITE, O_RDWR); //otwarcie pliku „in” sterownika
//  if(ss95el6_adres_write<0){
//     printf("Open %s – error: %d\n", ss95el6_ADRES_WRITE, errno);
//     exit(5);
//  }
//  snprintf(buffer, MAX_BUFFER, "128"); //treść do przekazania sterownikowi
//  int ss95el6_adres_write_zapis=write(ss95el6_adres_write, buffer, strlen(buffer)); //właściwe przekazanie danych
//  if(ss95el6_adres_write_zapis!=strlen(buffer)){ 
//  } 
//  close(ss95el6_adres_write);
//      
//  
//  test_int();
  
  
//   int ss95el2_adres_write=open(ss95el2_ADRES_WRITE, O_RDWR); //otwarcie pliku „in” sterownika
//   if(ss95el2_adres_write<0){
//     printf("Open %s – error: %d\n", ss95el2_ADRES_WRITE, errno);
//     exit(1);
//   }
  
//   int ss95el4_adres_write=open(ss95el4_ADRES_WRITE, O_RDWR); //otwarcie pliku „in” sterownika
//   if(ss95el4_adres_write<0){
//     printf("Open %s – error: %d\n", ss95el4_ADRES_WRITE, errno);
//     close(ss95el2_adres_write);
//     exit(3);
//   }
  
  
  
//   printf("Test zapisu - GpioConsole\n");
//   snprintf(buffer, MAX_BUFFER, "0000cccc"); //treść do przekazania sterownikowi
//   int ss95el2_adres_write_zapis=write(ss95el2_adres_write, buffer, strlen(buffer)); //właściwe przekazanie danych
//   if(ss95el2_adres_write_zapis!=strlen(buffer)){ 
//   }
//   snprintf(buffer, MAX_BUFFER, "55550000"); //treść do przekazania sterownikowi
//   int ss95el4_adres_write_zapis=write(ss95el4_adres_write, buffer, strlen(buffer)); //właściwe przekazanie danych
//   if(ss95el4_adres_write_zapis!=strlen(buffer)){ 
//   }   
  
//   close(ss95el2_adres_write);
//   close(ss95el4_adres_write);
//   sleep(2);
  
  
//   printf("Test odczytu\n");
//   int i = 0;
//   while(i < 20){
//     int ss95el1_adres_read=open(ss95el1_ADRES_READ, O_RDONLY); //otwarcie pliku „out” sterownika
//     if(ss95el1_adres_read<0){
//       printf("Open %s – error: %d\n", ss95el1_ADRES_READ, errno);
//       close(ss95el1_adres_read); //nikt inny nie zamknie wcześniej otwartego pliku
//       exit(2);
//     }
//     int ss95el3_adres_read=open(ss95el3_ADRES_READ, O_RDONLY); //otwarcie pliku „out” sterownika
//     if(ss95el3_adres_read<0){
//       printf("Open %s – error: %d\n", ss95el3_ADRES_READ, errno);
//       close(ss95el1_adres_read);
//       exit(4);
//     }
    
   
//     int ss95el1_adres_read_odczyt=read(ss95el1_adres_read, buffer, MAX_BUFFER); //odczyt danych ze sterownika
//     if(ss95el1_adres_read_odczyt>0){
//       buffer[ss95el1_adres_read_odczyt]='\0';
//       printf("Czujnik 1 %s\n", buffer);
//       }
      
//     int ss95el3_adres_read_odczyt=read(ss95el3_adres_read, buffer, MAX_BUFFER); //odczyt danych ze sterownika
//     if(ss95el3_adres_read_odczyt>0){
//       buffer[ss95el3_adres_read_odczyt]='\0';
//       printf("Czujnik 2 %s\n", buffer);
//       }
//       close(ss95el1_adres_read);
//       close(ss95el3_adres_read);
//       sleep(1);
//       i++;
//   }














// gpio_read_test();
gpio_write_test();
//   test_int();
  return 0; 

}



