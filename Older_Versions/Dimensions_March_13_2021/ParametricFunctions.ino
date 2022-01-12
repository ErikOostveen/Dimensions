void parametricFunctions() {
  // ***************************************************************************************************************************************
  // ***************************************************************************************************************************************                             
  // PARAMETRIC FUNCTIONS ******************************************************************************************************************
  // ***************************************************************************************************************************************
  // ***************************************************************************************************************************************

  // #######################################################################################################################################
  // param_a = 0 or 1   : Disable (0) or Enable (1) parameter A* 
  // param_b = 0 or 1   : Disable (0) or Enable (1) parameter A*
  // param_c = 0 or 1   : Disable (0) or Enable (1) parameter A* 
  // param_d = 0 or 1   : Disable (0) or Enable (1) parameter A* 
  // param_rnd = 0 or 1 : Disable (0) or Enable (1) parameter A*
  // px                 : Parametric equation acting on Interval
  // py                 : Parametric equation acting on Note   
  // pz                 : Parametric equation acting on Velocity
  //
  // NOTES:
  // *Parameters A, B, C, D and RND all can be set 0 (with the module's rotary knob) 
  // so make sure that in your equations, you are not causing a division by zero!
  // It might restart the Dimensions module you see; not great when you are right
  // in the middle of a jam session.
  //
  // If you intend to create more than 20 Paramteric Equations, then make sure to update
  // "numberOfParametricEQs" and "numberOfParametricEQs_fl" accordingly (in the "Dimensions_<month>_<day>_<year>" tab)
  // The defaults are as follows:
  // numberOfParametricEQs = 20;
  // numberOfParametricEQs_fl = 20;)
  // #######################################################################################################################################
  
      switch (fx_select) {
        case 1: // f(x) #1 (Lissajous) ================================================================================
  
          param_a = 1;
          param_b = 1;
          param_c = 1;
          param_d = 0;
          param_rnd = 0;
   
          px_fl = x + t * sin(a * t + PI/2); 
          py_fl = y + t * sin(b * t);
          pz_fl = t * cos(a * t/PI * c);
        
        break;
        
        case 2: // f(x) #2 ============================================================================================
  
          param_a = 1;
          param_b = 1;
          param_c = 0;
          param_d = 0;
          param_rnd = 0;
  
          px_fl = x + t * sin(a * t + PI/2) + cos(sq(a+b)/PI) * tan(a); 
          py_fl = y + t * sin(b * t) * ( tan((t * a)/a-b) / (b * 4) );
          pz_fl = t * cos(a * t/PI);
  
        break;
        
        case 3: // f(x) #3 ============================================================================================
  
          param_a = 1;
          param_b = 1;
          param_c = 1;
          param_d = 0;
          param_rnd = 0;
  
          px_fl = x + t * sin(a * t + PI/(c + 1)) + cos(sq(a+b)/(c + 0.5));
          py_fl = y + t * sin(c * t) * ( tan((t * a)/(c-b + 0.12)) / ((b * 4) + 0.5) );
          pz_fl = t * sin(a * t/PI); 
  
        break;
        case 4: // f(x) #4 ============================================================================================
  
          param_a = 1;
          param_b = 1;
          param_c = 1;
          param_d = 0;
          param_rnd = 0;
  
          px_fl = x + t * sin(a * t + PI/c) + cos(sq(PI/2)/c);
          py_fl = y + t * sin(b * t/2) * ( cos(t/PI * a) );
          pz_fl = t * sin(c * t/PI); 
  
        break;
        
        case 5: // f(x) #5 ============================================================================================
  
          param_a = 1;
          param_b = 1;
          param_c = 1;
          param_d = 0;
          param_rnd = 1;
  
          px_fl = x + t * sin(a * t/2) + ( rnd * ( PI/random(2,30) * 1.75 ) ); 
          py_fl = y + t * sin(b * t) + ( rnd * ( PI/random(5,25) * c ) );
          pz_fl = t * cos(a * t/PI * c); 
  
        break;
        
        case 6: // f(x) #6 ============================================================================================
  
          param_a = 0;
          param_b = 0;
          param_c = 0;
          param_d = 0;
          param_rnd = 0;
  
          px_fl = x + random(-64,64); 
          py_fl = y + random(-64,64);
          pz_fl = t * random(0,127); 
  
        break;
        
        case 7: // f(x) #7 ============================================================================================
  
          param_a = 1;
          param_b = 1;
          param_c = 1;
          param_d = 0;
          param_rnd = 0;
                                    
          px_fl = x + t * sin(a * t + PI/3) * (PI / pow(b,2));
          py_fl = y + t * sin(b * t);
          pz_fl = t * cos(a * t/PI * c * t);
  
        break;
        
        case 8: // f(x) #8 ============================================================================================
  
          param_a = 1;
          param_b = 1;
          param_c = 1;
          param_d = 0;
          param_rnd = 0;
   
          px_fl = x + t * sin(a * t + PI/2) - ( t * cos(a * t + PI/4) ); 
          py_fl = y + t * sin(b * t) + ( y - t * cos(0.25 * b * t) );
          pz_fl = t * cos(a * t/PI * c);
  
        break;
        
        case 9: // f(x) #9 ============================================================================================
  
          param_a = 1;
          param_b = 1;
          param_c = 1;
          param_d = 1;
          param_rnd = 0;
   
          px_fl = x + t * sin(a * t + PI/25) * (d * 0.5);         
          py_fl = y + t * cos(-b * t + PI/130) * (d * 0.5) + t * sin(a * t + PI/25);
          pz_fl = t * sin(t/PI * c);
  
        break;
        
        case 10: // f(x) #10 ===========================================================================================
  
          param_a = 1;
          param_b = 1;
          param_c = 1;
          param_d = 1;
          param_rnd = 0;

          px_fl = x + t * sin(a * t + PI/2) * log(16)/b * 2;
          py_fl = y + t * sin(b * t) + sin(d * t * 100) + tan(PI * t); 
          pz_fl = t * 0.5 * sin(t/PI * c + 0.25);
  
        break;

        case 11: // f(x) #11 (TEMPLATE) ================================================================================
  
          param_a = 0;
          param_b = 0;
          param_c = 0;
          param_d = 0;
          param_rnd = 0;
   
          px_fl = x; // Dot in the middle of the x-axis
          py_fl = y; // Dot in the middle of the y-axis
          pz_fl = 64; // Constant velocity at MIDI value 64 (at "vel. offst.: 0")
        
        break;

        case 12: // f(x) #12 (TEMPLATE) ================================================================================
  
          param_a = 0;
          param_b = 0;
          param_c = 0;
          param_d = 0;
          param_rnd = 0;
   
          px_fl = x; // Dot in the middle of the x-axis
          py_fl = y; // Dot in the middle of the y-axis
          pz_fl = 64; // Constant velocity at MIDI value 64 (at "vel. offst.: 0")
        
        break;

        case 13: // f(x) #13 (TEMPLATE) ================================================================================
  
          param_a = 0;
          param_b = 0;
          param_c = 0;
          param_d = 0;
          param_rnd = 0;
   
          px_fl = x; // Dot in the middle of the x-axis
          py_fl = y; // Dot in the middle of the y-axis
          pz_fl = 64; // Constant velocity at MIDI value 64 (at "vel. offst.: 0")
        
        break;

        case 14: // f(x) #14 (TEMPLATE) ================================================================================
  
          param_a = 0;
          param_b = 0;
          param_c = 0;
          param_d = 0;
          param_rnd = 0;
   
          px_fl = x; // Dot in the middle of the x-axis
          py_fl = y; // Dot in the middle of the y-axis
          pz_fl = 64; // Constant velocity at MIDI value 64 (at "vel. offst.: 0")
        
        break;

        case 15: // f(x) #15 (TEMPLATE) ================================================================================
  
          param_a = 0;
          param_b = 0;
          param_c = 0;
          param_d = 0;
          param_rnd = 0;
   
          px_fl = x; // Dot in the middle of the x-axis
          py_fl = y; // Dot in the middle of the y-axis
          pz_fl = 64; // Constant velocity at MIDI value 64 (at "vel. offst.: 0")
        
        break;

        case 16: // f(x) #16 (TEMPLATE) ================================================================================
  
          param_a = 0;
          param_b = 0;
          param_c = 0;
          param_d = 0;
          param_rnd = 0;
   
          px_fl = x; // Dot in the middle of the x-axis
          py_fl = y; // Dot in the middle of the y-axis
          pz_fl = 64; // Constant velocity at MIDI value 64 (at "vel. offst.: 0")
        
        break;

        case 17: // f(x) #17 (TEMPLATE) ================================================================================
  
          param_a = 0;
          param_b = 0;
          param_c = 0;
          param_d = 0;
          param_rnd = 0;
   
          px_fl = x; // Dot in the middle of the x-axis
          py_fl = y; // Dot in the middle of the y-axis
          pz_fl = 64; // Constant velocity at MIDI value 64 (at "vel. offst.: 0")
        
        break;

        case 18: // f(x) #18 (TEMPLATE) ================================================================================
  
          param_a = 0;
          param_b = 0;
          param_c = 0;
          param_d = 0;
          param_rnd = 0;
   
          px_fl = x; // Dot in the middle of the x-axis
          py_fl = y; // Dot in the middle of the y-axis
          pz_fl = 64; // Constant velocity at MIDI value 64 (at "vel. offst.: 0")
        
        break;

        case 19: // f(x) #19 (TEMPLATE) ================================================================================
  
          param_a = 0;
          param_b = 0;
          param_c = 0;
          param_d = 0;
          param_rnd = 0;
   
          px_fl = x; // Dot in the middle of the x-axis
          py_fl = y; // Dot in the middle of the y-axis
          pz_fl = 64; // Constant velocity at MIDI value 64 (at "vel. offst.: 0")
        
        break;

        case 20: // f(x) #20 (TEMPLATE) ================================================================================
  
          param_a = 0;
          param_b = 0;
          param_c = 0;
          param_d = 0;
          param_rnd = 0;
   
          px_fl = x; // Dot in the middle of the x-axis
          py_fl = y; // Dot in the middle of the y-axis
          pz_fl = 64; // Constant velocity at MIDI value 64 (at "vel. offst.: 0")
        
        break;
        
      }
  
  // ***************************************************************************************************************************************
  // ***************************************************************************************************************************************
  // ***************************************************************************************************************************************
  // ***************************************************************************************************************************************
  // ***************************************************************************************************************************************
}
