/**********************************************************************************************************************
File: user_app2.h                                                                

Description:
Header file for user_app2.c

**********************************************************************************************************************/

#ifndef __USER_APP2_H
#define __USER_APP2_H

/**********************************************************************************************************************
Type Definitions
**********************************************************************************************************************/


/**********************************************************************************************************************
Constants / Definitions
**********************************************************************************************************************/
#define TIMEDONE    (u32)60000
#define CODELENGTH  (u8)4

/*from userApp1*/
/**********************************************************************************************************************
Function Declarations
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Public functions                                                                                                   */
/*--------------------------------------------------------------------------------------------------------------------*/
static bool UserApp2_CompareCodes(void);

/*--------------------------------------------------------------------------------------------------------------------*/
/* Protected functions                                                                                                */
/*--------------------------------------------------------------------------------------------------------------------*/
void UserApp2Initialize(void);
void UserApp2RunActiveState(void);


/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/


/***********************************************************************************************************************
State Machine Declarations
***********************************************************************************************************************/
static void UserApp2SM_Idle(void);    

static void UserApp2SM_Error(void);         
static void UserApp2SM_FailedInit(void);        
static void UserApp2SM_SetPassword(void);
static void UserApp2SM_AttemptInProgress(void);
static void UserApp2SM_ResetGame(void);
static void UserApp2SM_Success(void);
static void UserApp2SM_Failure(void);

#endif /* __USER_APP2_H */


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
