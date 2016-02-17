//semaphore Tina <- 0
//semaphore Judy <- 0
//semaphore DontCareCusts <- 0
//semaphore TinaCusts <- 0
//semaphore JudyCusts <- 0
//N <- 5, customer limit per line
//int TinaSeat[N] <- array of size N
//int JudySeat[N] <- array of size N
//int DontCareSeat[N] <- array of size N
//
// P()<-wait() and V() <- signal()

//first solution: Tina
/**
 * int TinaNextServe <- 0
 * Mutex TinaSeatAccess <- 1
 * Tina {
 *
 *     int next_cust, cust_pid <- 0
 *
 *     while(true){
 *         P(Tina)
 *         P(TinaSeatAccess)
 *         TinaNextServe <- (TinaNextServe++) mod N //might be wrong for Tina and dont-care custs only
 *         next_cust <- TinaNextServe
 *         cust_pid <- TinaSeat[next_cust]
 *         TinaSeat[next_cust] <- system program id
 *         V(TinaSeatAccess)
 *         V(TinaCusts)
 *     }
 * }
 */


//second solution: Judy
/**
 * int JudyNextServe <- 0
 * Mutex JudySeatAccess <- 1
 * Judy {
 *
 *     int next_cust, cust_pid <- 0
 *
 *     while(true){
 *         P(Judy)
 *         P(JudySeatAccess)
 *         JudyNextServe <- (JudyNextServe++) mod N //might be wrong for Judy and dont-care custs only
 *         next_cust <- JudyNextServe
 *         cust_pid <- JudySeat[next_cust]
 *         JudySeat[next_cust] <- system program id
 *         V(JudySeatAccess)
 *         V(JudyCusts)
 *     }
 * }
 */


//third solution: Tina-only customers
/**
 * int TinaNextServe <- 0
 * Mutex TinaSeatAccess <- 1
 * TinaCustNextSeat <- 0
 * TinaCustomers {
 *     int FreeSeatCount <- N
 *     int cust_seat, barber_pid <- 0
 *     P(TinaSeatAccess)
 *     if (FreeSeatCount > 0) {
 *         FreeSeatCount <- FreeSeatCount - 1
 *         TinaCustNextSeat <- (TinaCustNextSeat++) mod N
 *         cust_seat <- TinaCustNextSeat
 *         TinaSeat[cust_seat] <- system program id
 *         V(TinaSeatAccess)
 *         V(Tina)
 *         P(TinaCusts)
 *         P(TinaSeatAccess)
 *         barber_pid <- TinaSeat[cust_seat]
 *         FreeSeatCount <- FreeSeatCount + 1
 *         V(TinaSeatAccess)
 *     } else {
 *         V(TinaSeatAccess)
 *     }
 *     exit thread
 * }
 */


//fourth solution: Judy-only customers
/**
 * int JudyNextServe <- 0
 * Mutex JudySeatAccess <- 1
 * JudyCustNextSeat <- 0
 * JudyCustomers {
 *     int FreeSeatCount <- N
 *     int cust_seat, barber_pid <- 0
 *     P(JudySeatAccess)
 *     if (FreeSeatCount > 0) {
 *         FreeSeatCount <- FreeSeatCount - 1
 *         JudyCustNextSeat <- (JudyCustNextSeat++) mod N
 *         cust_seat <- JudyCustNextSeat
 *         JudySeat[cust_seat] <- system program id
 *         V(JudySeatAccess)
 *         V(Judy)
 *         P(JudyCusts)
 *         P(JudySeatAccess)
 *         barber_pid <- JudySeat[cust_seat]
 *         FreeSeatCount <- FreeSeatCount + 1
 *         V(JudySeatAccess)
 *     } else {
 *         V(JudySeatAccess)
 *     }
 *     exit thread
 * }
 */


//fifth solution: Don't-care customers
/**
 * int DontCareNextServe <- 0
 * Mutex DontCareSeatAccess <- 1
 * DontCareCustNextSeat <- 0
 * DontCareCustomers {
 *     int FreeSeatCount <- N
 *     int cust_seat, barber_pid <- 0
 *     P(DontCareSeatAccess)
 *     if (FreeSeatCount > 0) {
 *         FreeSeatCount <- FreeSeatCount - 1
 *         DontCareCustNextSeat <- (DontCareCustNextSeat++) mod N
 *         cust_seat <- DontCareCustNextSeat
 *         DontCareSeat[cust_seat] <- system program id
 *         V(DontCareSeatAccess)
 *         V(DontCare)
 *         P(DontCareCusts)
 *         P(DontCareSeatAccess)
 *         barber_pid <- DontCareSeat[cust_seat]
 *         FreeSeatCount <- FreeSeatCount + 1
 *         V(DontCareSeatAccess)
 *     } else {
 *         V(DontCareSeatAccess)
 *     }
 *     exit thread
 * }
 */

