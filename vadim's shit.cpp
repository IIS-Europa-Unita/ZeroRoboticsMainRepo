//There are minuses idk how to fix

int blue;
int red;

float item_att[3];
float item_loc[3];
float loc_tofly[3];
float point_att[3];
float myState[12];
float dock_dis[3];
float startDis;
//float adapter_loc[2][3];
float sps_points[3][3];
float cubes[6][13];
float zone[4];
float near_of_zone[3];

int cur_cube;
//int item_tosetSPS;
int team;
int cur_sps;
int counter;
int k;

bool triangle_built;
bool going_around;
bool StartDisSaved;
bool flag;
bool zone_flag;

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

void init()
{
    
    triangle_built = false;
    counter = 0;
    cur_sps = 0;
    StartDisSaved = false;
    going_around = false;
    dock_dis[0] = 0.151;
	dock_dis[1] = 0.138;
	dock_dis[2] = 0.124;
    k = 0;
    
    blue = 1;
    red = 0;
    
    api.getMyZRState(myState);
    if (myState[1] == 0.15) team = blue;
    else team = red;
    //game.getItemZRState(adapter_loc[blue],7);
    //game.getItemZRState(adapter_loc[red],8);

    cur_cube = team;
    
    for(int i = 0; i < 3; ++ i) sps_points[0][i] = myState[i];
    

    game.getItemLoc(sps_points[2],team);
    DEBUG(("yellow loc"));
    float vadim = 0.1;
    for(int i=0; i<3; ++i)
    {
        DEBUG(("%f ",sps_points[2][i]));
        if (myState[i]<sps_points[2][i]) sps_points[2][i]-=vadim; else sps_points[2][i]+=vadim;
    }
    DEBUG(("yellow loc"));
    sps_points[1][2] = sps_points[2][2];
    float a = getDis(myState,sps_points[2]);
    if (a<=0.2) 
    {
        for(int i = 0; i < 3; ++ i) sps_points[0][i] * -1.5;
        a = getDis(sps_points[0],sps_points[2]);
    }
    float b = 1/(4*a);
    float k = b/a;
    sps_points[1][0] = k * sps_points[2][1];
    sps_points[1][1] = k * sps_points[2][0];
    sps_points[1][2] = sps_points[2][2];
    
    for (int i = 0; i< 3 ;++ i)
    {
        for (int j = 0; j < 3; j++)
        DEBUG(("%f ",sps_points[i][j]));
        DEBUG(("\n"));
    }
    
    for (int i = 0; i<=2; ++i)
        for(int j = 0; j <= 2; ++j)
            sps_points[i][j] = myState[j];
    
    sps_points[1][2] = myState[2]+0.6*pow(-1,team);
    
    sps_points[2][1] = myState[1]+0.5*pow(-1,team);
}

void triangle()
{
    if (cur_sps<3)
    {
    
        float arg = getDis(myState, sps_points[cur_sps]);
        api.setPositionTarget(sps_points[cur_sps]);
        //movement(sps_points[cur_sps]);
        //triangle_movement(sps_points[cur_sps]);
        DEBUG(("Dist == %f", arg));
        if (arg<=0.05)
        {
            DEBUG(("I dropped SPS"));
            game.dropSPS();
            cur_sps++;
        }
    }
    if (cur_sps == 3)
        {
            triangle_built = true;
            game.getZone(zone);
        }
}

/*void movement(float* target_copy)
{
    float targ_relative[3];
    float targ_absolute[3];
    for(int i =0; i<3; ++i)
    {
        targ_absolute[i] = target_copy[i];
       targ_relative[i] = targ_absolute[i] - myState[i];
    }
    
    float curDis = getDis(myState,targ_absolute);
    
    //if (curDis>0.7)
    //    api.setForces(targ_relative);
    //else
    //api.setPositionTarget(targ_absolute);
    
    DEBUG(("curDis is %f \n", curDis));
    if (curDis>=0.009)
    {
        if (!(StartDisSaved))
        {
            startDis = getDis(myState,targ_absolute);
            StartDisSaved = true;
        }
        if (curDis>(startDis))
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
}*/


void goAround(int num)
{
    float itemState[12];
    game.getItemZRState(itemState,num);
    float item_att[3];
    
    for(int i =0; i<3; ++i)
        item_att[i] = itemState[6+i];
    
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
        force_point[i] = item_att[i];
    force_point[max_num]*=-10;
    counter++;
    if (counter % 1 == 0)
    api.setForces(force_point);
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
        api.getMyZRState(myState);
        float myVel[3];
        for(int i=0; i < 3; ++i)
            myVel[i] = myState[i+3];
       
        float dis_tmp = getDis(myState,loc_tofly);
        float dis_tmp3 = getDis(myState,itemState);
        if ((dis_tmp>dis_tmp3) && (dis_tmp3<=0.3))
        {
            goAround(num);
        }
        //else
        //{
            //movement(loc_tofly);
           api.setPositionTarget(loc_tofly);
        //}
        api.setAttitudeTarget(point_att);
        DEBUG(("%f - vel \n", mathVecMagnitude(myVel,3)));
        DEBUG(("%f - dis \n", getDis(myState,item_loc)));
        if ((mathVecMagnitude(myVel,3) < 0.01) && (getDis(myState,item_loc)<=dock_dis[num / 2]+0.022) && (game.isFacingCorrectItemSide(num)) && (!(game.hasItem(num))))
        {
            game.dockItem(num);
            flag = true;
        }
    }
    if (game.hasItem(num) == 1)
    {
        float att[3];
        
        for (int i=0; i<3; i++)
        {
            att[i]=-myState[i]+zone[i];
        }
        
        if (zone_flag)
        {
            float l = getDis(myState,zone);
            
            for(int i =0;i < 3; i++)
            {
               near_of_zone[i] = 0.11*myState[i] / l + zone[i];
            }
            zone_flag = false;
        }
        api.setPositionTarget(near_of_zone);
        //movement(near_of_zone);
        api.setAttitudeTarget(att);
        for (int i =0; i<6; ++i)
            game.getItemZRState(cubes[i],i);
        if ((getDis(cubes[num],zone)<=0.06) && (fabsf(att[0])<=0.15) && (fabsf(att[1])<=0.15) && (fabsf(att[2])<=0.15))
        {
            game.dropItem();
            zone_flag = true;
            cur_cube ++;
        }
    }
}


void loop()
{
   
    api.getMyZRState(myState);
    
    DEBUG(("cur_sps= %d",cur_sps));
    
    if (triangle_built)
    {
        DEBUG(("TRIANGLE_BUILT == TRUE"));
        if (!game.itemInZone(cur_cube)) 
            takeCube(cur_cube);   
    }
    else 
    {
        triangle();
        DEBUG(("TRIANGLE_BUILT == FALSE"));
    }
}
