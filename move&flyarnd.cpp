void movement(float* target_copy)
{
    float relative[3];
    float absolute[3];
    for(int i =0; i<3; ++i)
    {
        absolute[i] = target_copy[i];
        relative[i] = absolute[i] - myState[i];
    }
    
    float curDis = getDis(myState, absolute);
    
    if (curDis >= 0.1) // distance on what it really shouldnt apply forces
    {
        if (counter % 4 == 0) api.setForces(relative); //frequency of applying forces. frequency is more when number is less 
        counter++;
    }
    api.setPositionTarget(absolute);
}


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
    force_point[max_num]*=-1;
    
    float c[3];
    mathVecAdd(c,force_point,item_att,3);
    mathVecNormalize(c,3);
    for(int i = 0; i < 3; ++ i)
        c[i]/=5;
    float b[3];
    mathVecAdd(b,loc_tofly,c,3);
    movement(b);
    
}



//lines to add in takeCube-function:
float x[3];
for(int i = 0; i < 3; ++ i)
            x[i] = item_att[i] + item_loc[i];
float a = getDis(myState, loc_tofly);
float b = getDis(x, loc_tofly);
float c = getDis(myState, x);
        
if (a*a+b*b<c*c)
    goAround(num);
else
    movement(loc_tofly);
