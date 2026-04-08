#include "stm32f10x.h"
#include "servo.h"
#include "delay.h"
#include "usart.h"
#include "variable.h"

/*
 * 功能：真正的非阻塞式摇尾巴函数
 * 说明：
 *   - 当WeiBa_Bit为1时执行摇尾巴
 *   - 每次调用只执行一小步，立即返回（不阻塞主循环）
 *   - 尾巴角度范围30-150度
 *   - WeiBa_Dir控制方向：1=向左摇，0=向右摇
 *   - WeiBa_Step_Timer控制速度
 */
void Action_SwingTail_NonBlocking(void)
{
    static uint8_t weiba_step_timer = 0;  /* 步计时器 */
    
    if(!WeiBa_Bit) 
    {
        return;  /* 没使能，直接返回 */
    }
    
    weiba_step_timer++;
    
    /* 每几次循环动一步，控制速度（越大越慢） */
    if(weiba_step_timer < 2)
    {
        return;
    }
    weiba_step_timer = 0;
    
    if(WeiBa_Dir)  /* 向左摇 */
    {
        WeiBa_Value++;
        if(WeiBa_Value >= 150)
        {
            WeiBa_Dir = 0;  /* 到顶，转向 */
            WeiBa_Value = 150;
        }
    }
    else  /* 向右摇 */
    {
        WeiBa_Value--;
        if(WeiBa_Value <= 30)
        {
            WeiBa_Dir = 1;  /* 到底，转向 */
            WeiBa_Value = 30;
        }
    }
    
    /* 设置舵机角度 */
    WServo_Angle(WeiBa_Value);
}

/*
 * 功能：阻塞式摇尾巴函数(已废弃，使用Action_SwingTail_NonBlocking代替)
 */
void Action_SwingTail(void)
{
}

/*
 * 功能：放松趴下动作
 * 说明：
 *   - 对应表情：Face_Mode=0(睡觉表情)
 *   - 前腿向前，后腿向后，呈放松趴下状态
 *   - 动作完成后切换到摇尾巴模式(Action_Mode=9)
 */
void Action_relaxed_getdowm(void)
{
    Face_Mode = 0;  /* 设置表情：睡觉 */
    Servo_Angle1(20);  /* 左前舵机20度 */
    Servo_Angle2(20);  /* 右前舵机20度 */
    delay_ms(80);
    Servo_Angle3(160); /* 左后舵机160度 */
    Servo_Angle4(160); /* 右后舵机160度 */

    Action_Mode=9;  /* 切换到摇尾巴模式 */
}

/*
 * 功能：站立动作
 * 说明：
 *   - 对应表情：Face_Mode=2(快乐表情)
 *   - 所有舵机回到90度中位，呈站立状态
 *   - 动作完成后切换到摇尾巴模式(Action_Mode=9)
 */
void Action_upright(void)
{
    Face_Mode = 2;  /* 设置表情：快乐 */
    Servo_Angle1(90);  /* 左前舵机90度 */
    Servo_Angle2(90);  /* 右前舵机90度 */
    delay_ms(80);
    Servo_Angle3(90);  /* 左后舵机90度 */
    Servo_Angle4(90);  /* 右后舵机90度 */

    Action_Mode=9;  /* 切换到摇尾巴模式 */
}

void Action_upright2(void)
{
    Servo_Angle3(90);
    Servo_Angle4(90);
    delay_ms(80);
    Servo_Angle1(90);
    Servo_Angle2(90);

    Action_Mode=9;
}

void Action_getdowm(void)
{
    Servo_Angle1(20);
    Servo_Angle2(20);
    delay_ms(80);
    Servo_Angle3(20);
    Servo_Angle4(20);

    Action_Mode=9;
}

/*
 * 功能：坐下动作
 * 说明：
 *   - 对应表情：Face_Mode=5(眼睛表情)
 *   - 前腿站立，后腿弯曲，呈坐下状态
 *   - 动作完成后切换到摇尾巴模式(Action_Mode=9)
 */
void Action_sit(void)
{
    Face_Mode = 5;  /* 设置表情：眼睛 */
    Servo_Angle1(90);  /* 左前舵机90度 */
    Servo_Angle2(90);  /* 右前舵机90度 */
    delay_ms(80);
    Servo_Angle3(20);  /* 左后舵机20度 */
    Servo_Angle4(20);  /* 右后舵机20度 */

    Action_Mode=9;  /* 切换到摇尾巴模式 */
}

/*
 * 功能：前进动作
 * 说明：
 *   - 对应表情：Face_Mode=4(非常快乐表情)
 *   - 四足交替运动实现前进
 *   - 支持单次运动和持续运动(通过Sustainedmove控制)
 *   - 运动完成后自动切换到站立状态(Action_Mode=2)
 */
void Action_advance(void)
{
    Face_Mode = 4;  /* 设置表情：非常快乐 */
    while(Action_Mode==4)  /* 只要Action_Mode是4就一直循环 */
    {
        PAnumbers=Chongfunumber;  /* 设置重复次数 */
        while((PAnumbers || Sustainedmove)&& Action_Mode==4)  /* 有重复次数或持续运动时执行 */
        {
            /* 第一组步态 */
            Servo_Angle2(45);
            Servo_Angle3(45);
            delay_ms(SpeedDelay);
            if(Action_Mode!=4)break;
            Servo_Angle1(135);
            Servo_Angle4(135);
            delay_ms(SpeedDelay);
            if(Action_Mode!=4)break;
            Servo_Angle2(90);
            Servo_Angle3(90);
            delay_ms(SpeedDelay);
            if(Action_Mode!=4)break;
            Servo_Angle1(90);
            Servo_Angle4(90);
            delay_ms(SpeedDelay);
            if(Action_Mode!=4)break;

            /* 第二组步态 */
            Servo_Angle1(45);
            Servo_Angle4(45);
            delay_ms(SpeedDelay);
            if(Action_Mode!=4)break;
            Servo_Angle2(135);
            Servo_Angle3(135);
            delay_ms(SpeedDelay);
            if(Action_Mode!=4)break;
            Servo_Angle1(90);
            Servo_Angle4(90);
            delay_ms(SpeedDelay);
            if(Action_Mode!=4)break;
            Servo_Angle2(90);
            Servo_Angle3(90);
            delay_ms(SpeedDelay);
            if(Action_Mode!=4)break;

            PAnumbers--;  /* 重复次数减1 */
        }
        if(Sustainedmove!=1 && Action_Mode==4)  /* 如果不是持续运动且还在前进模式 */
            Action_Mode=2;  /* 切换到站立状态 */
    }
}

/*
 * 功能：后退动作
 * 说明：
 *   - 对应表情：Face_Mode=1(瞪大眼表情)
 *   - 四足交替运动实现后退(与前进步态相反)
 *   - 支持单次运动和持续运动(通过Sustainedmove控制)
 *   - 运动完成后自动切换到站立状态(Action_Mode=2)
 */
void Action_back(void)
{
    Face_Mode = 1;  /* 设置表情：瞪大眼 */
    while(Action_Mode==5)  /* 只要Action_Mode是5就一直循环 */
    {
        PAnumbers=Chongfunumber;  /* 设置重复次数 */
        while((PAnumbers || Sustainedmove) && Action_Mode==5 )  /* 有重复次数或持续运动时执行 */
        {
            /* 第一组步态(与前进相反) */
            Servo_Angle2(135);
            Servo_Angle3(135);
            delay_ms(SpeedDelay);
            if(Action_Mode!=5)break;
            Servo_Angle1(45);
            Servo_Angle4(45);
            delay_ms(SpeedDelay);
            if(Action_Mode!=5)break;
            Servo_Angle2(90);
            Servo_Angle3(90);
            delay_ms(SpeedDelay);
            if(Action_Mode!=5)break;
            Servo_Angle1(90);
            Servo_Angle4(90);
            delay_ms(SpeedDelay);
            if(Action_Mode!=5)break;

            /* 第二组步态(与前进相反) */
            Servo_Angle1(135);
            Servo_Angle4(135);
            delay_ms(SpeedDelay);
            if(Action_Mode!=5)break;
            Servo_Angle2(45);
            Servo_Angle3(45);
            delay_ms(SpeedDelay);
            if(Action_Mode!=5)break;
            Servo_Angle1(90);
            Servo_Angle4(90);
            delay_ms(SpeedDelay);
            if(Action_Mode!=5)break;
            Servo_Angle2(90);
            Servo_Angle3(90);
            delay_ms(SpeedDelay);
            if(Action_Mode!=5)break;

            PAnumbers--;  /* 重复次数减1 */
        }
        if(Sustainedmove!=1 && Action_Mode==5)  /* 如果不是持续运动且还在后退模式 */
        Action_Mode=2;  /* 切换到站立状态 */

    }
}

void Action_Lrotation(void)
{
    while(Action_Mode==6)
    {
        PAnumbers=Chongfunumber;
        PAnumbers=PAnumbers+Chongfunumber;
        while((PAnumbers || Sustainedmove) && Action_Mode==6)
        {
            Servo_Angle2(45);
            Servo_Angle3(135);
            delay_ms(SpeedDelay);
            if(Action_Mode!=6)break;
            Servo_Angle1(45);
            Servo_Angle4(135);
            delay_ms(SpeedDelay);
            if(Action_Mode!=6)break;
            Servo_Angle2(90);
            Servo_Angle3(90);
            delay_ms(SpeedDelay);
            if(Action_Mode!=6)break;
            Servo_Angle1(90);
            Servo_Angle4(90);
            delay_ms(SpeedDelay);
            if(Action_Mode!=6)break;

            PAnumbers--;
        }
        if(Sustainedmove!=1 && Action_Mode==6)
        Action_Mode=2;
    }

}

void Action_Rrotation(void)
{
    while(Action_Mode==7)
    {
        PAnumbers=Chongfunumber;
        PAnumbers=PAnumbers+Chongfunumber;
        while((PAnumbers || Sustainedmove)  && Action_Mode==7)
        {
            Servo_Angle1(45);
            Servo_Angle4(135);
            delay_ms(SpeedDelay);
            if(Action_Mode!=7)break;
            Servo_Angle2(45);
            Servo_Angle3(135);
            delay_ms(SpeedDelay);
            if(Action_Mode!=7)break;
            Servo_Angle1(90);
            Servo_Angle4(90);
            delay_ms(SpeedDelay);
            if(Action_Mode!=7)break;
            Servo_Angle2(90);
            Servo_Angle3(90);
            delay_ms(SpeedDelay);
            if(Action_Mode!=7)break;

            PAnumbers--;
        }
        if(Sustainedmove!=1 && Action_Mode==7)
        Action_Mode=2;
    }

}

void Action_Swing(void)
{
    uint16_t SwingNumber=SwingRepeatnumber;
    while(SwingNumber && Action_Mode==8)
    {
        for(uint8_t i=30;i<150;i++)
        {
            Servo_Angle1(i);
            Servo_Angle2(i);
            Servo_Angle3(i);
            Servo_Angle4(i);
            delay_ms(SwingDelay);
            if(Action_Mode!=8)break;
        }
        if(Action_Mode!=8)break;
        for(uint8_t i=150;i>30;i--)
        {
            Servo_Angle1(i);
            Servo_Angle2(i);
            Servo_Angle3(i);
            Servo_Angle4(i);
            delay_ms(SwingDelay);
            if(Action_Mode!=8)break;
        }
        if(Action_Mode!=8)break;

        SwingNumber--;
    }
    for(uint8_t i=30;i<90;i++)
    {
        Servo_Angle1(i);
        Servo_Angle2(i);
        Servo_Angle3(i);
        Servo_Angle4(i);
        delay_ms(SwingDelay);
        if(Action_Mode!=8)break;
    }
    if(Action_Mode==8)
    Action_Mode=2;
}

void Action_JumpU(void)
{
    if(TiaoTurn==0)
    {
        Servo_Angle1(140);
        Servo_Angle4(35);
        delay_ms(SpeedDelay);

        Servo_Angle2(140);
        Servo_Angle3(35);
        delay_ms(SpeedDelay+80);

        Action_Mode=2;
        TiaoTurn=1;
    }
    else
    {
        Servo_Angle2(140);
        Servo_Angle3(35);
        delay_ms(SpeedDelay);

        Servo_Angle1(140);
        Servo_Angle4(35);
        delay_ms(SpeedDelay+80);

        Action_Mode=2;
        TiaoTurn=0;
    }
}

void Action_JumpD(void)
{
    if(TiaoTurn2==0){
        Servo_Angle4(35);
        Servo_Angle1(140);
        delay_ms(SpeedDelay);

        Servo_Angle3(35);
        Servo_Angle2(140);
        delay_ms(SpeedDelay);

        Action_Mode=12;
        TiaoTurn2=1;
    }
    else
    {
        Servo_Angle3(35);
        Servo_Angle2(140);
        delay_ms(SpeedDelay);

        Servo_Angle4(35);
        Servo_Angle1(140);
        delay_ms(SpeedDelay);

        Action_Mode=12;
        TiaoTurn2=0;
    }
}

/*
 * 功能：打招呼动作
 * 说明：
 *   - 对应表情：Face_Mode=6(打招呼表情)
 *   - 右前腿(舵机2)上下摆动实现挥手效果
 *   - HelloRepeatnumber控制挥手次数
 *   - 动作完成后自动切换到站立状态(Action_Mode=2)
 */
void Action_Hello(void)
{
    uint16_t HelloNumber=HelloRepeatnumber;  /* 获取挥手重复次数 */

    Face_Mode = 6;  /* 设置表情：打招呼 */
    Servo_Angle3(20);  /* 左后舵机20度 */
    Servo_Angle4(45);  /* 右后舵机45度 */
    delay_ms(80);
    Servo_Angle1(90);  /* 左前舵机90度 */
    while(HelloNumber && Action_Mode==13)  /* 还有挥手次数且模式不变时执行 */
    {
        if(Action_Mode!=13)break;
        /* 向上挥手 */
        for(int i=0;i<=45;i++)
        {
            if(Action_Mode!=13)break;
            Servo_Angle2(i);  /* 右前舵机从0到45度 */
            delay_ms(SwingDelay);
        }
        /* 向下挥手 */
        for(int i=45;i>0;i--)
        {
            if(Action_Mode!=13)break;
            Servo_Angle2(i);  /* 右前舵机从45到0度 */
            delay_ms(SwingDelay);
        }
        if(Action_Mode!=13)break;

        HelloNumber--;  /* 挥手次数减1 */
    }
    if(Action_Mode==13)  /* 如果还在打招呼模式 */
    Action_Mode=2;  /* 切换到站立状态 */
}

/*
 * 功能：伸懒腰动作
 * 说明：
 *   - 对应表情：Face_Mode=1(狂热表情)
 *   - 前腿先向前伸，再收回，然后后腿向后伸，再收回
 *   - 动作完成后自动切换到拉伸腿状态(Action_Mode=15)
 */
void Action_stretch(void)
{
    Face_Mode = 1;  /* 设置表情：狂热 */
    Servo_Angle3(90);  /* 左后舵机90度 */
    Servo_Angle4(90);  /* 右后舵机90度 */
    delay_ms(80);
    /* 前腿向前伸 */
    for(int i=90;i>10;i--)
    {
        Servo_Angle1(i);  /* 左前舵机从90到10度 */
        Servo_Angle2(i);  /* 右前舵机从90到10度 */
        if(Action_Mode!=14)break;
        delay_ms(15);
    }
    /* 前腿收回 */
    for(int i=10;i<90;i++)
    {
        Servo_Angle1(i);  /* 左前舵机从10到90度 */
        Servo_Angle2(i);  /* 右前舵机从10到90度 */
        if(Action_Mode!=14)break;
        delay_ms(15);
    }
    /* 后腿向后伸 */
    for(int i=90;i<170;i++)
    {
        Servo_Angle3(i);  /* 左后舵机从90到170度 */
        Servo_Angle4(i);  /* 右后舵机从90到170度 */
        if(Action_Mode!=14)break;
        delay_ms(15);
    }
    /* 后腿收回 */
    for(int i=170;i>90;i--)
    {
        Servo_Angle3(i);  /* 左后舵机从170到90度 */
        Servo_Angle4(i);  /* 右后舵机从170到90度 */
        if(Action_Mode!=14)break;
        delay_ms(15);
    }
    if(Action_Mode==14)  /* 如果还在伸懒腰模式 */
    Action_Mode=15;  /* 切换到拉伸腿状态 */
}

void Action_Lstretch(void)
{
    int breakvalue=1;
    int temp=3;
    while(breakvalue)
    {
        Servo_Angle1(90);
        Servo_Angle2(20);
        delay_ms(60);
        Servo_Angle4(110);
        for(int i=90;i<180;i++)
        {
            if(Action_Mode!=15)break;
            Servo_Angle3(i);
            delay_ms(6);
        }
        while(temp && Action_Mode==15)
        {
            for(int i=180;i>150;i--)
            {
                if(Action_Mode!=15)break;
                Servo_Angle3(i);
                delay_ms(15);
            }
            temp--;
        }
        if(Action_Mode!=15)break;
        delay_ms(100);
        Servo_Angle1(90);
        Servo_Angle2(90);
        if(Action_Mode!=15)break;
        delay_ms(80);
        Servo_Angle3(90);
        Servo_Angle4(90);
        delay_ms(100);
        if(Action_Mode!=15)break;

        temp=3;

        Servo_Angle2(90);
        Servo_Angle1(20);
        if(Action_Mode!=15)break;
        delay_ms(60);
        Servo_Angle3(110);
        for(int i=90;i<180;i++)
        {
            if(Action_Mode!=15)break;
            Servo_Angle4(i);
            delay_ms(6);
        }
        while(temp && Action_Mode==15)
        {
            for(int i=180;i>150;i--)
            {
                if(Action_Mode!=15)break;
                Servo_Angle4(i);
                delay_ms(15);
            }
            temp--;
        }
        if(Action_Mode==15)
        Action_Mode=2;


        breakvalue=0;
    }
}

void PetAction_Perform(void)
{
    if(Action_Mode==0){Action_relaxed_getdowm();}
    else if(Action_Mode==1){Action_sit();}
    else if(Action_Mode==2){Action_upright();}
    else if(Action_Mode==3){Action_getdowm();}
    else if(Action_Mode==4){Action_advance();}
    else if(Action_Mode==5){Action_back();}
    else if(Action_Mode==6){Action_Lrotation();}
    else if(Action_Mode==7){Action_Rrotation();}
    else if(Action_Mode==8){Action_Swing();}
    else if(Action_Mode==9){Action_SwingTail();}
    else if(Action_Mode==10){Action_JumpU();}
    else if(Action_Mode==11){Action_JumpD();}
    else if(Action_Mode==12){Action_upright2();}
    else if(Action_Mode==13){Action_Hello();}
    else if(Action_Mode==14){Action_stretch();}
    else if(Action_Mode==15){Action_Lstretch();}
}
