//
//  main.cpp
//  Ex3_Page_Replace_Algorithm
//
//  Created by 唐雨琪 on 2018/12/15.
//  Copyright © 2018 唐雨琪. All rights reserved.
//

#include <iostream>
#include <vector>
#include <time.h>
#include <iomanip>
#include <list>
#include <queue>
#include "Instructions.h"
#include "Page.h"
#define random(a,b) (rand()%(b-a+1)+a)
using namespace std;
vector<Instructions> insReference;
list<Page> mainMemory;//设内存一次能容纳4条指令,存放页a号
priority_queue<Page, vector<Page>, Page_2> qMM;
vector<Page> clockMemory;//用于clock算法
int clockPos=0;//用于clock算法中的循环队列
void InitializeInsSet();
void ObsoleteOPT(int curPos);
void ExecuteOPT();
void addPageOPT(int i);
void DisplayMM();
void addPageLFU(int i);
void ExecuteLFU();
void addPageLRU(int i);
void ExecuteLRU();
void ObsoleteLRU();
void addPageFIFO(int i);
void ExecuteFIFO();
void addPageClock(int i);
void ObsoleteClock();
void ExecuteClock();
int hit=0;
int main(int argc, const char * argv[]) {
    InitializeInsSet();
    while(1){
//        insReference.clear();
        mainMemory.clear();
        clockMemory.clear();
        while (qMM.empty()==false) {
            qMM.pop();
        }
        hit=0;
        int choice=0;
//        InitializeInsSet();
        cout<<"请选择算法（1=OPT;2=LFU;3=LRU;4=FIFO;5=CLOCK）:";
        cin>>choice;
        switch (choice) {
            case 1:
                ExecuteOPT();
                break;
            case 2:
                ExecuteLFU();
                break;
            case 3:
                ExecuteLRU();
                break;
            case 4:
                ExecuteFIFO();
                break;
            case 5:
                ExecuteClock();
                break;
            default:
                cout<<"输入错误"<<endl;
                continue;
        }
    }
    return 0;
}

//生成程序段
void InitializeInsSet(){
    vector<Instructions> program;
    program.clear();
    program.resize(128);//有128条指令的程序，假设每16条指令是1页,共8页
    for(int i=0;i<program.size();i++){
        program[i].Instruction_Id=i+1;
        program[i].Page=i/16+1;
    }
    srand((int)time(NULL));
    //在指令集合中随机选择一条指令执行
    insReference.resize(20);//有100条指令需要h执行(在这里设置数据量！！！！！！！！！）
    for(int i=0;i<insReference.size();i++){
        int  choice=random(0,127);//在以上指令集中随机选择一条执行
        insReference[i]=program[choice];
    }
    cout<<endl<<"页面序列<指令号：页面号>："<<endl;
    for(int i=0;i<insReference.size();i++){
        cout<<setw(3)<<insReference[i].Instruction_Id<<":"<<insReference[i].Page<<endl;
    }
//    cout<<endl;
}

void ExecuteOPT(){
    for(int i=0;i<insReference.size();i++){
        cout<<endl<<"执行指令"<<insReference[i].Instruction_Id<<"，所在页的页号"<<insReference[i].Page<<endl;
        list<Page>::iterator iter;
        cout<<"当前内存中页号："<<endl;
        for(iter=mainMemory.begin();iter!=mainMemory.end();iter++){
            cout<<iter->Page_Id<<endl;
        }
       
//        查找页面号
        for(iter=mainMemory.begin();iter!=mainMemory.end();iter++){
            if(iter->Page_Id==insReference[i].Page){
                
                break;
            }
        }
        if(mainMemory.size()<4){
            if(iter==mainMemory.end()){
                cout<<"内存有空余，";
                addPageOPT(i);
            }
            continue;
        }
        if(iter!=mainMemory.end()){
            hit++;
            cout<<"内存中存在该页面"<<endl;
        }
        else{
            cout<<"内存中不存在该页面"<<endl;
//            ObsoleteOPT(i);
            ObsoleteOPT(i);
            addPageOPT(i);
            
        }
    }
    cout<<"完成，命中率为"<<double(hit)/insReference.size()*100<<"%"<<endl;
}
//按照OPT算法淘汰
void ObsoleteOPT(int curPos){
//    cout<<"?????"<<mainMemory.size();
    list<Page>::iterator iter;
//    记录时间戳
    for(iter=mainMemory.begin();iter!=mainMemory.end();iter++){
        for(int i=(int)insReference.size();i>=curPos;i--){
            if(insReference[i].Page==iter->Page_Id){
                iter->Time_Stamp=i+1;
            }
        }
    }
    cout<<"时间戳:"<<endl;
    for(iter=mainMemory.begin();iter!=mainMemory.end();iter++){
        cout<<iter->Page_Id<<":"<<iter->Time_Stamp<<endl;
    }
//    cout<<endl;
//    找到在之后最久未使用的page
    list<Page>::iterator max=mainMemory.begin();
    for(iter=mainMemory.begin();iter!=mainMemory.end();iter++){
        if(iter->Time_Stamp>max->Time_Stamp){
            max=iter;
        }
    }
    cout<<"淘汰第"<<max->Page_Id<<"页，";
//    淘汰该页面
    mainMemory.erase(max);
    
}
//调入页面
void addPageOPT(int i){
    Page temp;
    temp.Page_Id=insReference[i].Page;
    temp.Time_Stamp=insReference.size();
    temp.Visited=-1;
    cout<<"调入第"<<temp.Page_Id<<"页"<<endl;
    mainMemory.push_back(temp);
}

//按照LRU算法淘汰
void ExecuteLFU(){
    for(int i=0;i<insReference.size();i++){
        cout<<endl<<"执行指令"<<insReference[i].Instruction_Id<<"，所在页的页号"<<insReference[i].Page<<endl;
        
        cout<<"当前内存中页号："<<endl;
        DisplayMM();
        priority_queue<Page, vector<Page>, Page_2> q(qMM);
        while (q.empty()==false) {
            if(q.top().Page_Id==insReference[i].Page){
                priority_queue<Page, vector<Page>, Page_2> q_temp;
                //查找遍历堆
                while (qMM.empty()==false) {
                    if(qMM.top().Page_Id==q.top().Page_Id){//在主存中找到了该页面
                        Page temp;
                        temp=qMM.top();
                        qMM.pop();
                        temp.Visited++;
                        qMM.push(temp);
                        break;
                    }
                    else{
                        q_temp.push(qMM.top());
                        qMM.pop();
                    }
                }
                while (q_temp.empty()==false) {
                    qMM.push(q_temp.top());
                    q_temp.pop();
                }
                break;
            }
            else{
                q.pop();
            }
        }
        
        if(qMM.size()<4){
            if(q.empty()!=false){
                cout<<"内存有空余，";
                addPageLFU(i);
            }
            continue;
        }
        if(q.empty()==false){
            hit++;
            cout<<"内存中存在该页面"<<endl;
        }
        else{
            cout<<"内存中不存在该页面"<<endl;
//            ObsoleteOPT(i);
            cout<<"淘汰第"<<qMM.top().Page_Id<<"页,";
            qMM.pop();
            addPageLFU(i);
            
        }
    }
    cout<<"完成，命中率为"<<double(hit)/insReference.size()*100<<"%"<<endl;
}
void addPageLFU(int i){
    Page temp;
    temp.Page_Id=insReference[i].Page;
    temp.Time_Stamp=-1;
    temp.Visited=1;
    cout<<"调入第"<<temp.Page_Id<<"页"<<endl;
    qMM.push(temp);
}
//用于LRU算法的内存显示
void DisplayMM(){
    priority_queue<Page, vector<Page>, Page_2> q(qMM);
    while (q.empty()==false) {
        cout<<q.top().Page_Id<<":"<<q.top().Visited<<endl;
        q.pop();
    }
}

void ExecuteLRU(){
    for(int i=0;i<insReference.size();i++){
        cout<<endl<<"执行指令"<<insReference[i].Instruction_Id<<"，所在页的页号"<<insReference[i].Page<<endl;
        list<Page>::iterator iter;
        cout<<"当前内存中页号："<<endl;
        for(iter=mainMemory.begin();iter!=mainMemory.end();iter++){
            cout<<iter->Page_Id<<":"<<iter->Time_Stamp<<endl;
        }
        
        //        查找页面号
        for(iter=mainMemory.begin();iter!=mainMemory.end();iter++){
            if(iter->Page_Id==insReference[i].Page){
//                mainMemory.erase(iter);
                iter->Time_Stamp=i;
                break;
            }
        }
        if(mainMemory.size()<4){
            if(iter==mainMemory.end()){
                cout<<"内存有空余，";
                addPageLRU(i);
            }
            continue;
        }
        if(iter!=mainMemory.end()){
//            iter->Visited=i;
            hit++;
            cout<<"内存中存在该页面"<<endl;
        }
        else{
            cout<<"内存中不存在该页面"<<endl;
            ObsoleteLRU();
            addPageLRU(i);
            
        }
    }
    cout<<"完成，命中率为"<<double(hit)/insReference.size()*100<<"%"<<endl;
}

void addPageLRU(int i){
    Page temp;
    temp.Page_Id=insReference[i].Page;
    temp.Time_Stamp=i;
    temp.Visited=-1;
    cout<<"调入第"<<temp.Page_Id<<"页"<<endl;
    mainMemory.push_back(temp);
}

void ObsoleteLRU(){
    list<Page>::iterator iter;
    list<Page>::iterator min=mainMemory.begin();
    for(iter=mainMemory.begin();iter!=mainMemory.end();iter++){
        if(iter->Time_Stamp<min->Time_Stamp){
            min=iter;
        }
    }
    cout<<"淘汰第"<<min->Page_Id<<"页，";
    //    淘汰该页面
    mainMemory.erase(min);
    
}

void ExecuteFIFO(){
    for(int i=0;i<insReference.size();i++){
        cout<<endl<<"执行指令"<<insReference[i].Instruction_Id<<"，所在页的页号"<<insReference[i].Page<<endl;
        list<Page>::iterator iter;
        cout<<"当前内存中页号："<<endl;
        for(iter=mainMemory.begin();iter!=mainMemory.end();iter++){
            cout<<iter->Page_Id<<endl;
        }
        
        //        查找页面号
        for(iter=mainMemory.begin();iter!=mainMemory.end();iter++){
            if(iter->Page_Id==insReference[i].Page){
                break;
            }
        }
        if(mainMemory.size()<4){
            if(iter==mainMemory.end()){
                cout<<"内存有空余，";
                addPageFIFO(i);
            }
            continue;
        }
        if(iter!=mainMemory.end()){
            hit++;
            cout<<"内存中存在该页面"<<endl;
        }
        else{
            cout<<"内存中不存在该页面"<<endl;
            cout<<"淘汰第"<<mainMemory.front().Page_Id<<"页，";
            mainMemory.pop_front();
            addPageFIFO(i);
            
        }
    }
    cout<<"完成，命中率为"<<double(hit)/insReference.size()*100<<"%"<<endl;
}

void addPageFIFO(int i){
    Page temp;
    temp.Page_Id=insReference[i].Page;
    temp.Time_Stamp=-1;
    temp.Visited=-1;
    cout<<"调入第"<<temp.Page_Id<<"页"<<endl;
    mainMemory.push_back(temp);
}

void ExecuteClock(){
    srand((int)time(NULL));
    for(int i=0;i<insReference.size();i++){
        cout<<endl<<"执行指令"<<insReference[i].Instruction_Id<<"，所在页的页号"<<insReference[i].Page<<endl;
        cout<<"当前内存中页号："<<endl;
        //此处借用时间戳字段表示修改位
        int k=0;
        //显示内存
        for(k=0;k<clockMemory.size();k++){
            cout<<clockMemory[k].Page_Id<<":"<<clockMemory[k].Visited<<":"<<clockMemory[k].Time_Stamp<<endl;
        }
        
        //查找
        for(k=0;k<clockMemory.size();k++){
            if(insReference[i].Page==clockMemory[k].Page_Id){
                //找到了
                break;
            }
        }
        if(clockMemory.size()<4){
            if(k==clockMemory.size()){
                cout<<"内存有空余，";
                addPageClock(i);
//                continue;
            }
            continue;
        }
        else{
            if(k!=clockMemory.size()){
                hit++;
                cout<<"内存中存在该页面"<<endl;
            }
            else{
                cout<<"内存中不存在该页面"<<endl;
                ObsoleteClock();
                addPageClock(i);
                
            }
        }
        
        
    }
    cout<<"完成，命中率为"<<double(hit)/insReference.size()*100<<"%"<<endl;
}
void addPageClock(int i){
    Page temp;
    temp.Page_Id=insReference[i].Page;
    temp.Time_Stamp=random(0, 1);//随机决定是否有血操作
    temp.Visited=1;
    cout<<"调入第"<<temp.Page_Id<<"页"<<endl;
    clockMemory.push_back(temp);
}

void ObsoleteClock(){
    int count=0;
    while(true){
        int posTemp=clockPos%clockMemory.size();
        cout<<"第"<<count+1<<"轮次扫描，"<<"当前指针位置："<<clockPos<<endl;
        clockPos=(clockPos+1)%clockMemory.size();
        //第一遍扫描
        for(int i=clockPos;i!=posTemp;i=(i+1)%clockMemory.size()){
            if(clockMemory[i].Visited==0&&clockMemory[i].Time_Stamp==0){
                cout<<"淘汰第"<<clockMemory[i].Page_Id<<"页";
                clockMemory.erase(clockMemory.begin()+i);
                return;
            }
        }
        count++;
        //第二轮扫描
        for(int i=clockPos;i!=posTemp&&clockMemory.size()!=0;i=(i+1)%clockMemory.size()){
            if(clockMemory[i].Visited==0&&clockMemory[i].Time_Stamp==1){
                cout<<"淘汰第"<<clockMemory[i].Page_Id<<"页";
                clockMemory.erase(clockMemory.begin()+i);
                return;
            }
            else{
                clockMemory[i].Visited=0;
            }
        }
    }
}
