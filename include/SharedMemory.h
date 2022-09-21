#ifndef SHAREDMEMORY_H
#define SHAREDMEMORY_H

#include "Common.h"

//#define EXTERNAL
#define LOCAL

#define MAX_COMMAND_DATA        40

typedef union{
    struct{
        unsigned    FET:1;	 	// FET ON   //
        unsigned    RUN:1;		// Control ON
        unsigned    INIT:1;     // Init Process Passed  //
        unsigned    MOD:1;		// Control Mode
        unsigned    NON_CTR:1;		// Nonius Count err //
        unsigned    BAT:1;      // Low Battery //
        unsigned    CALIB:1;    // Calibration Mode //
        unsigned    MT_ERR:1;      // Reply Status //

        unsigned    JAM:1;		// JAM Error
        unsigned    CUR:1;		// Over Current Error
        unsigned    BIG:1;		// Big Position Error
        unsigned    INP:1;      // Big Input Error
        unsigned    FLT:1;		// FET Driver Fault Error //
        unsigned    TMP:1;      // Temperature Error //
        unsigned    PS1:1;		// Position Limit Error (Lower) ////
        unsigned    PS2:1;		// Position Limit Error (Upper)

        unsigned    rsvd:8;

        unsigned    KP:16;
        unsigned    KD:16;
    }b;
    unsigned char B[7];
}mSTAT;

typedef struct _TASK_INFO_ {
    Eigen::Vector3f pos;
    Eigen::Vector3f vel;
    Eigen::Vector3f acc;
    Eigen::Vector3f force;
    Eigen::Vector4f quat;
    Eigen::Matrix3f rot;
    Eigen::Vector3f rpy;
    Eigen::Vector3f omega;
    Eigen::Matrix3f Jacobian;
    Eigen::Vector3f kp;
    Eigen::Vector3f kd;
} TASK_INFO;

typedef struct _JOINT_INFO_ {
    float pos;
    float vel;
    float acc;
    float torque;
    float kp;
    float kd;
} JOINT_INFO;

typedef struct _ROBOT_INFO_ {
    TASK_INFO   W_body, B_body, Brp_body, Brpy_body;
    TASK_INFO   W_leg[MAX_LEG], B_leg[MAX_LEG], Brp_leg[MAX_LEG], Brpy_leg[MAX_LEG];
    Eigen::Vector4i    contact;
    JOINT_INFO  joint[MAX_JOINT];
} ROBOT_INFO;

typedef struct _MOTOR_INFO_ {
    float pos;
    float vel;
    float acc;
    float cur;
    char  temp;
    mSTAT status;
    bool connect;
} MOTOR_INFO;


typedef struct _IMU_INFO_ {
    Eigen::Vector4f quat;
    Eigen::Vector3f rpy;
    Eigen::Vector3f gyro;
    Eigen::Vector3f acc;
} IMU_INFO;


typedef struct _JOY_INFO_ {
    float L_UD, L_RL, R_UD, R_RL;
    float LT, RT;
    bool BTN[15];
    bool status;
} JOY_INFO;

typedef struct _SENSOR_INFO_ {
    MOTOR_INFO motor[MAX_MC];
    IMU_INFO imu;
    JOY_INFO joy;
} SENSOR_INFO;


///-------------------------------------------------------------------------------
///---------------------Communication Struct--------------------------------------
///-------------------------------------------------------------------------------

// from GUI to Upboard
typedef struct _USER_COMMAND_
{
    int     USER_COMMAND;
    char    USER_PARA_CHAR[MAX_COMMAND_DATA];
    int     USER_PARA_INT[MAX_COMMAND_DATA];
    float   USER_PARA_FLOAT[MAX_COMMAND_DATA];
    double  USER_PARA_DOUBLE[MAX_COMMAND_DATA];
} USER_COMMAND, *pUSER_COMMAND;

// Upboard to GUI
typedef struct _ROBOT_STATE_DATA_ {
    ROBOT_INFO   State;
    ROBOT_INFO   ControlRef;
    ROBOT_INFO   CommandRef;
    SENSOR_INFO  Sensor;
    MOTOR_INFO   motor_ref[MAX_MC];
    float        custom_variable[20];
} ROBOT_STATE_DATA, *pROBOT_STATE_DATA;

// one-time transfer to GUI
typedef struct _ONE_TIME_DATA_ {
    int one_time_transfer_flag;
    USER_COMMAND cmd;
} ONE_TIME_DATA, pONE_TIME_DATA;
///-------------------------------------------------------------------------


typedef struct _RBCORE_SHM_
{
    bool LanComm_Status;
    int LanComm_Mode;
    bool NEWCOMMAND;
    int mc_ch;

    ROBOT_STATE_DATA ROBOT_DATA;
    ONE_TIME_DATA BASIC_ONE_DATA;
    ONE_TIME_DATA MPC_ONE_DATA;
    USER_COMMAND  COMMAND;

}RBCORE_SHM, *pRBCORE_SHM;



#endif // SHAREDMEMORY_H
