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

//code to put in taking cubes function:
    float dis1 = getDis(myState,loc_tofly); //dis to point near of face
    float dis2 = getDis(myState,itemState); //dis to cube's center
    if ((dis1>dis2) && (dis2<=0.4))
        goAround(cube_num);
    else
        movement(loc_tofly);
