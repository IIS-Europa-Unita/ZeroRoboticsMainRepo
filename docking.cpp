//There are minuses idk how to fix

float item_att[3];
float item_loc[3];
float loc_tofly[3];
float point_att[3];
float myState[12];
float dock_dis[6];

void init()
{
    dock_dis[0] = 0.173;
	dock_dis[1] = 0.160;
	dock_dis[2] = 0.146;
} 

float getDis(float* pos1, float* pos2) 
{
    float tmp;
    float qdist=0;
    for(int i=0; i<3; i++)
        tmp = (pos2[i]-pos1[i]);
        qdist+=(tmp*tmp);
    return sqrt( qdist );
}

void takeCube(int num)
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
    api.setPositionTarget(loc_tofly);
    api.setAttitudeTarget(point_att);
    api.getMyZRState(myState);
    float myVel[3];
    for(int i=0; i < 3; ++i)
    {
        myVel[i] = myState[i+3];
    }
    api.setPositionTarget(loc_tofly);
    api.setAttitudeTarget(point_att);
    if ((mathVecMagnitude(myVel,3) < 0.01) && (getDis(myState,item_loc)<=dock_dis[num / 2]) && (game.isFacingCorrectItemSide(num)) && (!(game.hasItem(num)))) game.dockItem(num);
}

void loop()
{
    api.getMyZRState(myState);
    takeCube(2);
}
