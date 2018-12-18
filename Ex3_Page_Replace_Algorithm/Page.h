//
//  Page.h
//  Ex3_Page_Replace_Algorithm
//
//  Created by 唐雨琪 on 2018/12/17.
//  Copyright © 2018 唐雨琪. All rights reserved.
//

#ifndef Page_h
#define Page_h
struct Page{
    int Page_Id;
    int Time_Stamp;
    int Visited;
    
    Page(int a,int b,int c){
        Page_Id=a;
        Time_Stamp=b;
        Visited=c;
    }
    Page(){
        Page_Id=0;
        Time_Stamp=0;
        Visited=0;
    }
};
struct Page_2{
    bool operator() (Page a,Page b)
    {
        return a.Visited > b.Visited; //小顶堆
    }
};

#endif /* Page_h */
