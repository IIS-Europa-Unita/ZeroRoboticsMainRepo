int counter;

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
    
    if (curDis>0.7)
        api.setForces(targ_relative);
    else
        api.setPositionTarget(targ_absolute);
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
    counter++;
    if (counter % 2 == 0) api.setForces(force_point);
}

dock_dis[0] = 0.17;
dock_dis[1] = 0.157;
dock_dis[2] = 0.143;


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
    loc_tofly[i] = item_att[i]*dock_dis[num / 2]/length + item_loc[i];
float dis_to_point_near = getDis(myState,point_near);
float dis_to_center = getDis(myState,itemState);
if ((dis_to_point_near>dis_to center) && (dis_to_point_nea<=0.4))
{
    DEBUG(("OBLET"));
    goAround(num);
}
    else
{
    movement(loc_tofly);
}
