int blue;
int red;

float item_att[3];
float item_loc[3];
float loc_tofly[3];
float point_att[3];
float myState[12];
float dock_dis[6];
float startDis;
float adapter_loc[2][3];
float sps_points[3][3];
float cubes[6][3];
float zone[4];

int case_case;
int item_tosetSPS;
int team;
int cur_sps;
int counter;

bool triangle_built;
bool going_around;
bool StartDisSaved;

void init()
{
    counter = 0;
    cur_sps = 0;
    StartDisSaved = false;
    going_around = false;
    dock_dis[0] = 0.173;
	dock_dis[1] = 0.160;
	dock_dis[2] = 0.146;
    
    blue = 0;
    red = 1;
    
    api.getMyZRState(myState);
    if (myState[1] == 0.15) team = blue;
    else team = red;
    game.getItemZRState(adapter_loc[blue],7);
    game.getItemZRState(adapter_loc[red],8);
    
    for (int i =0; i<6; ++i)
        game.getItemZRState(cubes[i],i); //Здесь он меняется на труъ
    
    triangle_built = false; // поменяем обратно)
    
    if (getDis(adapter_loc[team],myState)>=0.3)
        case_case = 1;
    else case_case = 2;
    
    //float dis[3];
    //float max = getDis(adapter_loc[team], cubes[team]);
   // int num_max = team;
    
    sps_points[0][0] = 0;
    sps_points[0][1] = myState[1];
    sps_points[0][2] = 0;
    
    sps_points[1][0] = 0;
    sps_points[1][1] = myState[1];
    sps_points[1][2] = myState[2]+0.6;
    
    sps_points[2][0] = 0;
    sps_points[2][1] = myState[1]*5;
    sps_points[2][2] = sps_points[1][2];
    /*switch (case_case)
    {
        case 1:
            sps_points[0][0] = 0;
            sps_points[0][1] = myState[1];
            sps_points[0][2] = 0;
            
            sps_points[1][0] = adapter_loc[team][0];
            sps_points[1][1] = adapter_loc[team][1];
            sps_points[1][2] = adapter_loc[team][2];
            
            max = getDis(adapter_loc[team], cubes[team]);
            for(int i =1; i<3; ++i)
            {
               dis[i] = getDis(adapter_loc[team], cubes[i+team]);
               if (dis[i]>max)
               {
                   max = dis[i];
                   num_max = i;
               }
            }
            item_tosetSPS = num_max;
            
            for (int i=0; i<3; ++i)
            {
                sps_points[2][i] = cubes[item_tosetSPS][i];
                if (myState[i]>cubes[item_tosetSPS][i]) sps_points[2][i]+=0.1;
                if (myState[i]<cubes[item_tosetSPS][i]) sps_points[2][i]-=0.1;
            }
            break;
        case 2:
            sps_points[0][0] = adapter_loc[team][0];
            sps_points[0][1] = adapter_loc[team][1];
            sps_points[0][2] = adapter_loc[team][2];
            
            if (adapter_loc[team][0]>cubes[team][0])
            {
                sps_points[1][0] = -0.6;
                sps_points[1][1] = 0;
                sps_points[1][2] = 0;
            }
            else
            {
                sps_points[1][0] = 0.6;
                sps_points[1][1] = 0;
                sps_points[1][2] = 0;
            }
            for (int i=0; i<3; ++i)
            {
                sps_points[2][i] = cubes[team][i];
                if (myState[i]>cubes[team][i]) sps_points[2][i]+=0.1;
                if (myState[i]<cubes[team][i]) sps_points[2][i]-=0.1;
            }
            break;
    }*/
} 

float getDis(float* pos1, float* pos2) 
{
    float tmp;
    float qdist=0;
    for(int i=0; i<3; i++)
    {
        tmp = (pos2[i]-pos1[i]);
        qdist+=(tmp*tmp);
    }
    return sqrt( qdist );
}


void DropSPS()
{
    if (cur_sps!=3)
    {
        float arg = getDis(myState, sps_points[cur_sps]);
        //movement(sps_points[cur_sps]);
        api.setPositionTarget(sps_points[cur_sps]);
        DEBUG(("Dist == %f", arg));
        if (arg<=0.05)
        {
            DEBUG(("I dropped SPS"));
            game.dropSPS();
            cur_sps++;
        }
    }
    if ((!triangle_built) && (cur_sps == 3))
        {
            triangle_built = true;
            game.getZone(zone);
        }
}

void movement(float* target_copy)
{
    float targ_relative[3];
    float targ_absolute[3];
    for(int i =0; i<3; ++i)
    {
        targ_absolute[i] = target_copy[i];
        targ_relative[i] = targ_absolute[i] - myState[i];
    }
    
    float curDis = getDis(myState,targ_absolute);
    DEBUG(("curDis is %f \n", curDis));
    if (curDis>=0.009)
    {
        if (!(StartDisSaved))
        {
            startDis = getDis(myState,targ_absolute);
            StartDisSaved = true;
            DEBUG(("Start distance is saved, it is %f \n", startDis));
        }
        if (curDis>(startDis/1.6))
        {
            api.setForces(targ_relative);
            DEBUG(("We are accellerating \n"));
        }
        else
        {
            api.setPositionTarget(targ_absolute); 
        }
    }
    else
    {
        StartDisSaved = false;
    }
}


void goAround(int num)
{
    float itemState[12];
    game.getItemZRState(itemState,num);
    float item_att[3];
    
    for(int i =0; i<3; ++i)
        item_att[i] = itemState[6+i] / 2;
    
    float dist[3];
    float max=-1;
    int max_num=-1;
    for(int i = 0; i<3; ++i)
    {
        dist[i] = fabsf(myState[i]-itemState[i]);
        if (dist[i]>max)
        {
            max = dist[i];
            max_num = i;
        }
    }
    
    float force_point[3];
    for(int i =0; i<3;++i)
        force_point[i] = item_att[i] / 10;
    force_point[max_num]*=-1;
    counter++;
    if (counter % 2 == 0) api.setForces(force_point);
}

void takeCube(int num)
{
    if (game.hasItem(num) == 0)
    {
        float itemState[12];
        game.getItemZRState(itemState,num);
        for(int i = 0; i<3; ++i)
        {
            item_att[i] = itemState[6+i];
            item_loc[i] = itemState[i];
            point_att[i] = -item_att[i];
        }
    	mathVecNormalize(item_att,3);
        float length = mathVecMagnitude(item_att,3);
        for(int i = 0; i<3; ++i)
            loc_tofly[i] = item_att[i]*dock_dis[num / 2]/length + item_loc[i]; //i can tell u how it works
        //movement(loc_tofly);
        api.setPositionTarget(loc_tofly);
        api.setAttitudeTarget(point_att);
        api.getMyZRState(myState);
        float myVel[3];
        for(int i=0; i < 3; ++i)
            myVel[i] = myState[i+3];
       
        float dis_tmp = getDis(myState,loc_tofly);
        float dis_tmp3 = getDis(myState,itemState);
        if (dis_tmp>dis_tmp3)
        {
            DEBUG(("OBLET"));
            goAround(num);
        }
        else if (going_around) goAround(num);
        else //movement(loc_tofly);
            api.setPositionTarget(loc_tofly);
        api.setAttitudeTarget(point_att);
        DEBUG(("%f - vel \n", mathVecMagnitude(myVel,3)));
        DEBUG(("%f - dis \n", getDis(myState,item_loc)));
        if ((mathVecMagnitude(myVel,3) < 0.01) && (getDis(myState,item_loc)<=dock_dis[num / 2]) && (game.isFacingCorrectItemSide(num)) && (!(game.hasItem(num))))
        {
            game.dockItem(num);
        }
    }
    if (game.hasItem(num) == 1)
    {
        float near_of_zone[3];
        float att[3];
        
        for (int i=0; i<3; i++)
        {
            near_of_zone[i]=zone[i];
            att[i]=-myState[i]+near_of_zone[i];
        }
        
        for(int i =0;i < 3; i++)
        {
            if (myState[i]>near_of_zone[i]) near_of_zone[i]+=0.07;
            else near_of_zone[i]-=0.07;
        }
        api.setPositionTarget(near_of_zone);
        //movement(near_of_zone);
        api.setAttitudeTarget(att);
        for (int i =0; i<6; ++i)
            game.getItemZRState(cubes[i],i);
        if ((getDis(cubes[num],zone)<=0.1) && (fabsf(att[0])<=0.1) && (fabsf(att[1])<=0.1) && (fabsf(att[2])<=0.1))
        {
            game.dropItem();     
        }
    }
}


void loop()
{
    //for (int i =0; i<6; ++i)
    //    game.getItemZRState(cubes[i],i);
    api.getMyZRState(myState);
    DEBUG(("%d", cur_sps));
    if (triangle_built)
    {
        DEBUG(("TRIANGLE_BUILT == TRUE"));
        if (!game.itemInZone(0)) 
            takeCube(0);   
    }
    else 
    {
        DropSPS();
        DEBUG(("TRIANGLE_BUILT == FALSE"));
    }
}
