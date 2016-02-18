//Initial assumptions about haircuts:
// every thread is paired with a distinct process id
// every thread can access its own process id and will be referenced by type (Tina, Judy, Dont-Care)
// the barber will store the customer's process id while cutting their hair
// the customer stores the barber's pid while getting their hair cut
//
//Initial assumptions about environment of operation:
// threads are released by a semaphore in the queuing order

//assumed there are three separate lines (3 queues), namely Tina-Only, Judy-Only and Dont-Care-Only,
//and that Dont-Care customers go to their line unless it has more than 5 people, then they will go to either Tina's
//or Judy's line depending on if either has less than 5 people (checking in that order). Don't Care customers will
//wake up Tina if their line is not full or if their line is full and the Tina line is not full when they arrive. They will wake up
//Judy only if the Judy line is the only available (non-full) line. Obviously Tina will be awake if her line is full, so that
//is another assumption.
//mod is short for modulo operation.

/*
//counting semaphore
semaphore Barbers <- 0

//binary semaphores
semaphore DontCareCusts <- 0
semaphore TinaCusts <- 0
semaphore JudyCusts <- 0

int N <- 5, integer customer limit per line
int TinaSeats[N] <- empty integer array of size N
int JudySeats[N] <- empty integer array of size N
int DontCareSeats[N] <- empty integer array of size N

//used for determining a random hair cut time to pick
int TIME_CAP <- 90 minutes

//track the free seats for each line with separate counters
int TinaFreeSeats <- N
int TinaCustNextSeat <- 0
Mutex TinaSeatAccess <- 1
int TinaCustNextSeat <- 0

int JudyFreeSeats <- N
int JudyCustNextSeat <- 0
Mutex JudySeatAccess <- 1
int JudyCustNextSeat <- 0

int DontCareFreeSeats <- N
int DontCareCustNextSeat <- 0
Mutex DontCareSeatAccess <- 1
int DontCareCustNextSeat <- 0
*/

//first solution: Tina
// the thread for Tina must be started by the main program
/**
 * int TinaNextServe <- 0
 * Mutex TinaSeatAccess <- 1
 * Tina {
 *
 *     int next_cust, cust_pid <- 0
 *
 *     while(true){
 *     	   //sleep until awoke by customer
 *         wait(Barbers)
 *
 *         //protect seat changes in Tina line
 *         wait(TinaSeatAccess)
 *
 *         //cut Tina hair or signal ability to modify line
 *         if (TinaFreeSeats < N) {
 *             doTina()
 *         } else {
 *             signal(TinaSeatAccess)
 *         }
 *
 *         //protect dont-care seat changes
 *         wait(DontCareSeatAccess)
 *
 *         //cut Dont-care hair or signal ability to modify line
 *         if (DontCareFreeSeats < N) {
 *             doDontCare()
 *         } else {
 *             signal(DontCareSeatAccess)
 *         }
 *     }
 * }
 */


//second solution: Judy
// the thread for Judy must be started by the main program
/**
 * int JudyNextServe <- 0
 * Mutex JudySeatAccess <- 1
 * Judy {
 *
 *     int next_cust, cust_pid <- 0
 *
 *     while(true){
 *     	   //sleep until awoke by customer
 *         wait(Barbers)
 *
 *         //protect seat changes
 *         wait(JudySeatAccess)
 *
 *         //try to cut Judy customer hair
 *         if (JudyFreeSeats < N) {
 *             doJudy()
 *         } else {
 *             signal(JudySeatAccess)
 *         }
 *
 *         //protect seat changes
 *         wait(DontCareSeatAccess)
 *
 *         if (DontCareFreeSeats < N){
 *             doDontCare()
 *         } else {
 *             signal(DontCareSeatAccess)
 *         }
 *     }
 * }
 */


//third solution: Tina-only customers
// threads for Tina's Customers should be started at random times for continuous customer approach
/**
 * TinaCustomer {
 *     int cust_seat, barber_pid <- 0
 *
 *     //protect seat changes with mutex
 *     wait(TinaSeatAccess)
 *
 *     //stay if there are less than 5 people (up to 5 free seats total)
 *     if (TinaFreeSeats > 0) {
 *         doTinaCustomer()
 *     } else {
 *         //free the seat change mutex
 *         signal(TinaSeatAccess)
 *     }
 *     //customer either had hair cut or left
 *     exit current thread or process
 * }
 */


//fourth solution: Judy-only customers
// threads for Judy's Customers should be started at random times for continuous customer approach
/**
 * JudyCustomer {
 *     int cust_seat, barber_pid <- 0
 *
 *     //protect seat changes with mutex
 *     wait(JudySeatAccess)
 *
 *     //stay if there are less than 5 people (up to 5 free seats total)
 *     if (JudyFreeSeats > 0) {
 *         doJudyCustomer()
 *     } else {
 *         //free the seat change mutex
 *         signal(JudySeatAccess)
 *     }
 *     //customer either had hair cut or left
 *     exit current thread or process
 * }
 */


//fifth solution: Don't-Care customers
// threads for Don't-Care Customers should be started at random times for continuous customer approach
/**
 * DontCareCustomer {
 *     int cust_seat, barber_pid <- 0
 *
 *     //attempt to access don't-care line
 *     wait(DontCareSeatAccess)
 *     if (DontCareSeats > 0) {
 *         doDontCareCustomer()
 *     } else {
 *         signal(DontCareSeatAccess)
 *     }
 *
 *     //attempt to access Tina line
 *     wait(TinaSeatAccess)
 *     if (TinaSeats > 0) {
 *         doTinaCustomer()
 *     }  else {
 *         signal(TinaSeatAccess)
 *     }
 *
 *     //attempt to access Judy line
 *     wait(JudySeatAccess)
 *     if (JudySeats > 0) {
 *         doJudyCustomer()
 *     } else {
 *         signal(JudySeatAccess)
 *     }
 *     exit current thread or process
 * }
 */

/**
 * doTina(){
 *      //choose the next customer
 *      TinaNextServe <- TinaNextServe mod N //might be wrong for Tina and dont-care custs only
 *      next_cust <- TinaNextServe
 *
 *      //increment Tina's next customer
 *		TinaNextServe++
 *
 *      //find the customer pid
 *      cust_pid <- TinaSeats[next_cust]
 *
 *      //leave customer Tina's pid
 *      TinaSeats[next_cust] <- Tina's process id
 *
 *      //allow seat changes by releasing mutex
 *      signal(TinaSeatAccess)
 *
 *      //wake up customer
 *      signal(TinaCusts)
 *
 *      //Tina cuts the hair of customer C
 * }
 */

/**
 * doJudy(){
 *      //choose the next customer
 *      JudyNextServe <- JudyNextServe mod N //might be wrong for Judy and dont-care custs only
 *      next_cust <- JudyNextServe
 *
 *      //increment Judy's next customer
 *		JudyNextServe++
 *
 *     //find the customer pid
 *     cust_pid <- JudySeats[next_cust]
 *
 *     //leave customer Judy's pid
 *     JudySeats[next_cust] <- Judy's process id
 *
 *     //allow seat changes by releasing mutex
 *     signal(JudySeatAccess)
 *
 *     //wake up customer
 *     signal(JudyCusts)
 *
 *     //Judy cuts the hair of customer C
 * }
 */


/**
 * The Tina line routine to execute when there are Tina-only seats open.
 * doTinaCustomer(){
 *     //take a seat in the Tina line
 *     TinaFreeSeats <- TinaFreeSeats - 1
 *
 *     //increment the customer seat position
 *     TinaCustNextSeat <- TinaCustNextSeat mod N
 *
 *     //chose the new seat position
 *     cust_seat <- TinaCustNextSeat
 *
 *	   //increment the customer seat id
 *	   TinaCustNextSeat++
 *
 *     //store the customer's pid
 *     TinaSeats[cust_seat] <- customer's process id
 *
 *     //free the seat change mutex
 *     signal(TinaSeatAccess)
 *
 *     //wake up Tina
 *     signal(Tina)
 *
 *     //wait in the Tina line
 *     wait(TinaCusts)
 *     wait(TinaSeatAccess)
 *
 *     //store Tina's pid
 *     barber_pid <- TinaSeats[cust_seat]
 *
 *     //allow seat to be used for more customers
 *     TinaFreeSeats <- TinaFreeSeats + 1
 *     signal(TinaSeatAccess)
 *
 *     //customer has hair cut by barber
 * }
 */

/**
 * The Judy line routine to execute when there are Judy-only seats open.
 * doJudyCustomer(){
 *     //take a seat in the Judy line
 *     JudyFreeSeats <- JudyFreeSeats - 1
 *
 *     //increment the customer seat position
 *     JudyCustNextSeat <- JudyCustNextSeat mod N
 *
 *     //chose the new seat position
 *     cust_seat <- JudyCustNextSeat
 *
 *	   //increment the customer seat id
 *	   JudyCustNextSeat++
 *
 *     //store the customer's pid
 *     JudySeats[cust_seat] <- customer's process id
 *
 *     //free the seat change mutex
 *     signal(JudySeatAccess)
 *
 *     //wake up Judy
 *     signal(Judy)
 *
 *     //wait in the Judy line
 *     wait(JudyCusts)
 *     wait(JudySeatAccess)
 *
 *     //store Judy's pid
 *     barber_pid <- JudySeats[cust_seat]
 *
 *     //allow seat to be used for more customers
 *     JudyFreeSeats <- JudyFreeSeats + 1
 *     signal(JudySeatAccess)
 *
 *     //customer has hair cut by Judy with barber_pid
 * }
 */

/**
 * The don't-care line routine to execute when there are dont-care seats open.
 * doDontCareCustomer(){
 *
 *     //take a seat in don't care line
 *     DontCareSeats <- DontCareSeats - 1
 *
 *     //get the current customers index
 *     DontCareCustNextSeat <- DontCareCustNextSeat mod N
 *
 *     //store the seat index
 *     cust_seat <- DontCareCustNextSeat
 *
 *     //increment the next seat position
 *     DontCareCustNextSeat++
 *
 *     //store the customer's process id at that index seat
 *     DontCareSeats[cust_seat] <- customer's process id
 *
 *     //give back seat access
 *     signal(DontCareSeatAccess)
 *
 *     //wake up Tina first (shop owner)
 *     signal(Tina)
 *
 *     //wait in the dont care custs line
 *     wait(DontCareCusts)
 *     wait(DontCareSeatAccess)
 *
 *     //retrieve the barber's pid
 *     barber_pid <- DontCareSeats[cust_seat]
 *
 *     //increment the free seat count
 *     FreeSeatCount <- FreeSeatCount + 1
 *
 *     //signal an open dont-care seat
 *     signal(DontCareSeatAccess)
 *
 *     //customer gets hair cut by Tina or Judy
 * }
 */

