
/**
 * @name IOS projekt 2
 * @file proj2.c 
 * @author Marek Miček (xmicek08)
 * @date 22.4. 2020
 */

#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>
#include <time.h>

void set_resources();
void delete_resources();

FILE *f;

// semaphores
sem_t *sem_printer;
sem_t *sem_main_process;
sem_t *sem_judge_in;
sem_t *sem_all_checked;
sem_t *sem_end_confirm;
sem_t *sem_judge_out;


//shared memory variables
int *A;     ///< proces number
int *I;     ///< process's ID in his category
int *NE;	///< imigrants without decision and registration
int *NC;	///< imigrants without decision but with registration
int *NB;	///< num of imigrants in building

int *JUDGE_IN; 	///< indicates if judge in building, if zero judge is out, if one then inside
int *IMIGRANT_OUT;	///< number of imigrants waiting to enter
int *IMIGRANT_IN;	///< number of imigrants waiting to leave

int PI, IG, JG, IT, JT;		///< arguments

/**
 * @brief Parses arguments from user, when invalid number or bad format => exit(1)
 * @param argc Number of arguments from user
 * @param argv Pointer to array of arguments from user
 * @return Void
 */
void get_arguments(int argc, char **argv)
{
	char *parse;
	if (argc != 6)
	{
		fprintf(stderr, "Invalid number of arguments\n");
		exit(1);
	}
	else
	{
		PI = strtol(argv[1], &parse, 10);
		if (PI < 1 || *parse != '\0')
		{
			fprintf(stderr, "Argument PI requires integer: PI <= 1!\n");
			exit(1);
		}

		IG = strtol(argv[2], &parse, 10);
		if (IG < 0 || IG > 2000 || *parse != '\0')
		{
			fprintf(stderr, "Argument IG requires integer : 0 <= IG <= 2000!\n");
			exit(1);
		}

		JG = strtol(argv[3], &parse, 10);
		if (JG < 0 || JG > 2000|| *parse != '\0')
		{
			fprintf(stderr, "Argument JG requires integer : 0 <= JG <= 2000!\n");
			exit(1);
		}

		IT = strtol(argv[4], &parse, 10);
		if (IT < 0 || IT > 2000 || *parse != '\0')
		{
			fprintf(stderr, "Argument IT requires integer : 0 <= IT <= 2000!\n");
			exit(1);
		}

		JT = strtol(argv[5], &parse, 10);
		if (JT < 0 || JT > 2000 || *parse != '\0')
		{
			fprintf(stderr, "Argument JT requires integer : 0 <= IG <= 2000!\n");
			exit(1);
		}
	}
		// conversion from miliseconds to microseconds
		IG *= 1000;
		JG *= 1000;
		IT *= 1000;
		JT *= 1000;
}

/**
 * @brief Opens the file for writing result, when not able open file => exit(1)
 * @param file Pointer to file to be opened
 * @return Void
 */
void open_file(FILE **file)
{
	*file = fopen("proj2.out", "w");
	if (file == NULL)
	{
		fprintf(stderr, "Error while opening file\n");
		exit(1);
	}
}

/**
 * @brief Initializes semaphores and shared variables, when not able to create semaphores or shared variables => exit(1)
 * @return Void
 */
void set_resources()
{
	/************ Setting semaphores **************/
	sem_printer = sem_open("/xmicek08.print_semafor", O_CREAT | O_EXCL, 0666, 1);
	sem_main_process = sem_open("/xmicek08.main_semafor", O_CREAT | O_EXCL, 0666, 0);
	sem_judge_in = sem_open("/xmicek08.judge_semafor", O_CREAT | O_EXCL, 0666, 0);
	sem_all_checked = sem_open("/xmicek08.checking_semafor", O_CREAT | O_EXCL, 0666, 0);
	sem_end_confirm = sem_open("/xmicek08.confirm_semafor", O_CREAT | O_EXCL, 0666, 0);
	sem_judge_out = sem_open("/xmicek08.judge_out_semafor", O_CREAT | O_EXCL, 0666, 0);

	if (sem_printer == SEM_FAILED || sem_main_process == SEM_FAILED || sem_judge_in == SEM_FAILED || sem_all_checked == SEM_FAILED || sem_end_confirm ==SEM_FAILED || sem_judge_out == SEM_FAILED)
	{
		fclose(f);
		fprintf(stderr, "Error when opening semaphore\n");
		exit(1);
	}

	/************ Setting shared variables *********/
	A = (int*)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	I = (int*)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	NE = (int*)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	NC = (int*)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	NB = (int*)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	JUDGE_IN = (int*)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	IMIGRANT_OUT = (int*)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	IMIGRANT_IN = (int*)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	if (A == MAP_FAILED || I == MAP_FAILED || NE == MAP_FAILED || NC == MAP_FAILED || NB == MAP_FAILED || JUDGE_IN == MAP_FAILED || IMIGRANT_OUT == MAP_FAILED || IMIGRANT_IN == MAP_FAILED)
	{
		fclose(f);
		delete_resources();
		fprintf(stderr, "Error while setting shared variables\n");
		exit(1);
	}

	*A = 1;
	*I = 0;
	*NE = 0;
	*NC = 0;
	*NB = 0;
	*JUDGE_IN = 0;
	*IMIGRANT_OUT = 0;
	*IMIGRANT_IN = 0;
}

/**
 * @brief Deallocates and deletes semaphores and shared variables
 * @return Void
 */
void delete_resources()
{
	sem_close(sem_printer);
	sem_close(sem_main_process);
	sem_close(sem_judge_in);
	sem_close(sem_all_checked);
	sem_close(sem_end_confirm);
	sem_close(sem_judge_out);

	sem_unlink("/xmicek08.print_semafor");
	sem_unlink("/xmicek08.main_semafor");
	sem_unlink("/xmicek08.judge_semafor");
	sem_unlink("/xmicek08.checking_semafor");
	sem_unlink("/xmicek08.confirm_semafor");
	sem_unlink("/xmicek08.judge_out_semafor");

	/************ Unmaping shared variables *******/
	munmap(A, sizeof(int));
	munmap(I, sizeof(int));
	munmap(NE, sizeof(int));
	munmap(NC, sizeof(int));
	munmap(NB, sizeof(int));
	munmap(JUDGE_IN, sizeof(int));
	munmap(IMIGRANT_OUT, sizeof(int));
	munmap(IMIGRANT_IN, sizeof(int));
}

/**
 * @brief Simulates imigrant process, ends with exit(0)
 * @return Void
 */
void imigrant_process()
{
	(*I)++;		///< increment actual id of imigrant process
	int id = *I;

	sem_wait(sem_printer);
	fprintf(f, "%d\t %s %d\t %s\n", *A, ": IMM", id, ": starts" );
	(*A)++;
	sem_post(sem_printer);

	if (*JUDGE_IN == 1)				///< judge is in building
	{
		(*IMIGRANT_OUT)++;			///< inc number of imigrants waiting to enter
		sem_wait(sem_judge_in);		///< waits until judge leaves building
	}
			
	sem_wait(sem_printer);
	(*NE)++;
	(*NB)++;
	fprintf(f, "%d\t %s %d\t %s\t :%d\t :%d\t :%d\n", *A, ": IMM", id, ": enters", *NE, *NC, *NB);
	(*A)++;
	sem_post(sem_printer);

				
	sem_wait(sem_printer);
	(*NC)++;
	fprintf(f, "%d\t %s %d\t %s\t :%d\t :%d\t :%d\n", *A, ": IMM", id, ": checks", *NE, *NC, *NB);
	(*A)++;
	sem_post(sem_printer);
				
	if (*NE == *NC && *JUDGE_IN == 1)		///< if judge waits for imigrant's checking
	{
		sem_post(sem_all_checked);	///< allow judge to start confirmation
	}
					
	sem_wait(sem_end_confirm);	///< waits until judge makes decision

	sem_wait(sem_printer);
	fprintf(f, "%d\t %s %d\t %s\t :%d\t :%d\t :%d\n", *A, ": IMM", id, ": wants certificate", *NE, *NC, *NB);
	(*A)++;
	sem_post(sem_printer);

	if (IT != 0)
		usleep(rand() % IT);     ///< process is set to sleep on the basis of argument IT

	sem_wait(sem_printer);
	fprintf(f, "%d\t %s %d\t %s\t :%d\t :%d\t :%d\n", *A, ": IMM", id, ": got certificate", *NE, *NC, *NB);
	(*A)++;
	sem_post(sem_printer);
					

	if (*JUDGE_IN == 1)					///< if judge is in building
	{
		(*IMIGRANT_IN)++;
		sem_wait(sem_judge_out);		///< waits until judge leaves building
	}
				
	sem_wait(sem_printer);
	(*NB)--;
	fprintf(f, "%d\t %s %d\t %s\t :%d\t :%d\t :%d\n", *A, ": IMM", id, ": leaves", *NE, *NC, *NB);
	(*A)++;
	sem_post(sem_printer);

	sem_post(sem_main_process);		///< the main process will end as the last one
	exit(0);
}	

/**
 * @brief Simulates the judge process, ends with exit(0)
 * @return Void
 */ 
void judge_process()
{
	int CONFIRMED = 0;				///< number of imigrants with confirmed request
	while (CONFIRMED < PI)
	{	

		if (JG != 0)
			usleep(rand() % JG);	///< process is set to sleep on the basis of argument JG

		sem_wait(sem_printer);
		fprintf(f, "%d\t %s\t %s\n", *A, ": JUDGE", ": wants to enter");
		(*A)++;
		sem_post(sem_printer);

		sem_wait(sem_printer);
		fprintf(f, "%d\t %s\t %s\t :%d\t :%d\t :%d\n", *A, ": JUDGE", ": enters", *NE, *NC, *NB);
		(*A)++;
		sem_post(sem_printer);

		*JUDGE_IN = 1;	///<judge is in building

		if (*NE != *NC)
		{
			sem_wait(sem_printer);
			fprintf(f, "%d\t %s\t %s\t :%d\t :%d\t :%d\n", *A, ": JUDGE", ": waits for imm", *NE, *NC, *NB);
			(*A)++;
			sem_post(sem_printer);

			sem_wait(sem_all_checked);	///< judge waits until *NE == *NC
		}

		sem_wait(sem_printer);
		fprintf(f, "%d\t %s\t %s\t :%d\t :%d\t :%d\n", *A, ": JUDGE", ": starts confirmation", *NE, *NC, *NB);
		(*A)++;
		sem_post(sem_printer);

		if (JT != 0)
			usleep(rand() % JT);	///< process is set to sleep on the basis of argument JT

		int checked = *NC;			///< storing the value of checked imm. for unlocking the semaphore sem_end_confirm
		CONFIRMED += *NC;			///< store the number of confirmed imigrants so judge knows when he doesn't need to enter

		*NE = 0;
		*NC = 0;
		sem_wait(sem_printer);
		fprintf(f, "%d\t %s\t %s\t :%d\t :%d\t :%d\n", *A, ": JUDGE", ": ends confirmation", *NE, *NC, *NB);
		(*A)++;
		sem_post(sem_printer);

		for (int i = 0; i < checked; i++)
			sem_post(sem_end_confirm);		///< allows imimgrants to go for their certificates

		if (JT != 0)
			usleep(rand() % JT);	///< process is set to sleep on the basis of argument JT

		sem_wait(sem_printer);
		fprintf(f, "%d\t %s\t %s\t :%d\t :%d\t :%d\n", *A, ": JUDGE", ": leaves", *NE, *NC, *NB);
		(*A)++;
		sem_post(sem_printer);


		for (int j = 0; j < *IMIGRANT_OUT; j++)		///< allows imigrants to go in
			sem_post(sem_judge_in);

		for (int k = 0; k < *IMIGRANT_IN; k++)		///< allows imigrants with certificates to go out
			sem_post(sem_judge_out);

		*JUDGE_IN = 0;		///< judge is not in building
		*IMIGRANT_OUT = 0;	///< no imigrants waiting outside
		*IMIGRANT_IN = 0;	///< no imigrants with certificates waiting inside
	}			
			
	sem_wait(sem_printer);
	fprintf(f, "%d\t %s\t %s\n", *A, ": JUDGE", ": finishes");
	(*A)++;
	sem_post(sem_printer);

	sem_post(sem_main_process);		///< the main process will end as the last one
	exit(0);	
}


int main(int argc, char **argv){

	pid_t mainPid, imigrantPid, judgePid;
	

		get_arguments(argc, argv);
		open_file(&f);
		set_resources();

		setbuf(f, NULL);
		setbuf(stderr, NULL);
		srand(time(0));

		// forking main process, new subprocess for generating imigrants is created
		if ((mainPid = fork()) < 0)
		{
			delete_resources();
			fprintf(stderr, "Error while forking main process\n");
			fclose(f);
			exit(1);
		}

		if (mainPid == 0)   ///< child process for generating imigrants
		{
			for (int i=0; i < PI; i++)
			{
				if (IG != 0)
					usleep(rand() % IG);     ///< process is set to sleep on the basis of argument IG

				imigrantPid = fork();		///< generating the process imigrant
				if (imigrantPid < 0)
				{
					delete_resources();
					fprintf(stderr, "Error while forking imigrant process\n");
					fclose(f);
					exit(1);
				}

				if (imigrantPid == 0)	///< child process imigrant
				{
					imigrant_process();
				}
			}
			exit(0);
		}

		// forking judge process, new subprocess judge is created
		if ((judgePid = fork()) < 0)
		{
			delete_resources();
			fprintf(stderr, "Error while forking judge process\n");
			fclose(f);
			exit(1);	
		}

		if (judgePid == 0)	///< child process judge
		{
			judge_process();
		}

		for (int i = 0; i < PI+1; i++)		///< main process waits until all imigrants and judge end
			sem_wait(sem_main_process);


		delete_resources();
		fclose(f);

		exit(0);
}
