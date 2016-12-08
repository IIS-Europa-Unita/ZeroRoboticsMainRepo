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
}
