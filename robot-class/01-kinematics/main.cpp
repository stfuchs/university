#include <Ors/roboticsCourse.h>


void simpleArrayOperations(){
  arr x;
  x = ARR(.1, .2, .3);         //directly setting the array
  cout <<"x = " <<x <<endl;

  x += ARR(2., 2., 2.);        //adding to an array
  cout <<"x = " <<x <<endl;

  x *= 1.;
  cout <<"x = " <<x <<endl;

  arr y = ARR(-.3,-.2,-.1);
  y.append(x);                 //appending a vector to a vector
  cout <<"y = " <<y <<endl;

  arr M(4,3);                  //some 3 x 4 matrix
  M[0] = ARR(1, 0, 0);         //setting the first row
  M[1] = ARR(0, 1, 0);
  M[2] = ARR(0, 0, 1);
  M[3] = ARR(1, 0, 0);

  cout <<"M =\n" <<M <<endl;

  cout <<"transpose M =\n" <<~M <<endl;  //matrix transpose

  cout <<"M*x = " <<M*x <<endl;          //matrix-vector product

  cout <<"M^-1 =\n" <<inverse(M) <<endl; //matrix inverse

  M.append(M);                 //appending a matrix to a matrix
  cout <<"M =\n" <<M <<endl;
}


void openingSimulator(){
  Simulator S("man.ors");
  cout <<"joint dimensions = " <<S.getJointDimension() <<endl;

  cout <<"initial posture (hit ENTER in the OpenGL window to continue!!)" <<endl;
  S.watch();        //pause and watch initial posture

  arr q;
  S.getJointAngles(q);

  q(0) += 0.1;                 //change the first entry of q-vector
  S.setJointAngles(q);
  S.watch();
  
  q = 0.;                      //set q-vector equal zero
  S.setJointAngles(q);
  S.watch();
}

void reachFast()
{
  Simulator S("man.ors");
  arr q,W;
  uint n = S.getJointDimension();
  S.getJointAngles(q);
  W.setId(n);  //W is equal the Id_n matrix
  S.watch();        //pause and watch initial posture

  uint T = 10;
  arr y_target,y,J;
  arr X(T,2);

  for(uint i=0;i<T;++i)
  {
    y_target = ARR(-0.2, -0.4, 1.1);
    S.kinematicsPos(y,"handR");  //"handR" is the name of the right hand ("handL" for the left hand)
    S.jacobianPos  (J,"handR");

    X(i,0) = i;
    X(i,1) = length(y_target - y);

    q += inverse(~J*J + 1e-4*W)*~J*(y_target - y);
    S.setJointAngles(q);
    S.watch();
  }
  gnuplot(X);
}

void reachSmooth()
{
  Simulator S("man.ors");
  arr q,W;
  uint n = S.getJointDimension();
  S.getJointAngles(q);
  W.setId(n);  //W is equal the Id_n matrix
  S.watch();        //pause and watch initial posture

  uint T = 100;
  arr y_target,y,J;
  arr X(T,2);

  for(uint i=0;i<T;++i)
  {
    y_target = ARR(-0.2, -0.4, 1.1);
    S.kinematicsPos(y,"handR");  //"handR" is the name of the right hand ("handL" for the left hand)
    S.jacobianPos  (J,"handR");

    X(i,0) = i;
    X(i,1) = length(y_target - y);

    q += .1 * inverse(~J*J + 1e-4*W)*~J*(y_target - y);
    S.setJointAngles(q);
    S.watch();
  }
  gnuplot(X);
}


void reachLinear()
{
  Simulator S("man.ors");
  arr q,W;
  uint n = S.getJointDimension();
  S.getJointAngles(q);
  W.setId(n);  //W is equal the Id_n matrix
  S.watch();        //pause and watch initial posture

  uint T = 100;
  arr y_target,y,y0,yt,J;
  arr X(T,2);
  S.kinematicsPos(y0,"handR");
  for(uint i=0;i<T;++i)
  {
    y_target = ARR(-0.2, -0.4, 1.1);
    S.kinematicsPos(y,"handR");  //"handR" is the name of the right hand ("handL" for the left hand)
    S.jacobianPos  (J,"handR");

    X(i,0) = i;
    X(i,1) = length(y_target - y);

    yt = y0 + (double(i)/double(T))*(y_target-y0);
    q += .1 * inverse(~J*J + 1e-4*W)*~J*(yt-y); 

    S.setJointAngles(q);
    S.watch();
  }
  gnuplot(X);
}


void reachCircle(){
  Simulator S("man.ors");
  arr q,W;
  uint n = S.getJointDimension();
  S.getJointAngles(q);
  W.setId(n);  //W is equal the Id_n matrix
  S.watch();        //pause and watch initial posture

  uint T = 100;
  arr c,y,yt,y0,J;
  arr X(T,2);
  S.kinematicsPos(y0,"handR");
  float sigma = 2.f*M_PI/float(T);
  arr R = { cos(sigma), 0, -sin(sigma),
            0, 1., 0,
            sin(sigma), 0, cos(sigma) };
  R.reshape(3,3);
  c = ARR(-0.2, -0.4, 1.1);
  yt = (y0 - c) / length(y0-c) * .2;

  for(uint i=0;i<1000;i++){
    S.kinematicsPos(y,"handR");  //"handR" is the name of the right hand ("handL" for the left hand)
    S.jacobianPos  (J,"handR");

    //X(i,0) = i;
    //X(i,1) = length(c - y);

    yt = R*yt;
    q += .1 * inverse(~J*J + 1e-4*W)*~J*(yt + c - y);

    S.setJointAngles(q);
    //S.watch();
  }
  //gnuplot(X);
}

void multiTask(){
  Simulator S("man.ors");
  arr q,q_home,y_target,y,J,v_target,v,v0,vt,Jv,W,Phi,PhiJ;
  uint n = S.getJointDimension();
  S.getJointAngles(q);
  W.setDiag(1.,n); //we define W as identity matrix
  q_home = q; //we store the initial posture as q_home
  S.watch();

  S.kinematicsVec(v0,"handL");
  v0 = v0 / length(v0);
  v_target = ARR(0,0,1.);
  double omega = acos((~v0*v_target)(0));

  for(uint i=0;i<10000;i++){
    Phi.clear();
    PhiJ.clear();
    //1st task: track with right hand
    y_target = ARR(-0.2, -0.4, 1.1);
    y_target += .2 * ARR(cos((double)i/20), 0, sin((double)i/20));
    S.kinematicsPos(y,"handR");
    S.jacobianPos(J,"handR");

    double t = double(std::min(int(i),50))/50.d;
    double denom = 1./sin(omega);
    vt = (denom*sin( (1.-t)*omega )) * v0 + (denom*sin(t*omega)) * v_target;
    //v_target = v_target / length(v_target);

    S.kinematicsVec(v,"handL");
    S.jacobianVec(Jv,"handL");

    Phi .append( 1e2 * (y - y_target) );
    PhiJ.append( 1e2 * J );
    //rho = 1e4 (cp. slide 02:45)
    //add the "stay close to home" task here
    Phi .append( 1e-1 * (q - q_home) );
    PhiJ.append( 1e-1 * W );

    //add another task for the left hand here, if you like
    Phi .append( 1e0 * ( v - vt) );
    PhiJ.append( 1e0 * Jv );

    //compute joint updates
    q -= inverse(~PhiJ*PhiJ + W)*~PhiJ* Phi; //(cp. slide 02:46)
    S.setJointAngles(q);
  }
}

int main(int argc,char **argv){
  MT::initCmdLine(argc,argv);

  switch(MT::getParameter<int>("mode",2))
  {
  case 0:
    cout << "Mode: simpleArrayOperations()" << endl;
    simpleArrayOperations();
    break;
  case 1:
    cout << "Mode: openingSimulator()" << endl;
    openingSimulator();
    break;
  case 2:
    cout << "Mode: reachFast()" << endl;
    reachFast();
    break;
  case 3:
    cout << "Mode: reachSmooth()" << endl;
    reachSmooth();
    break;
  case 4:
    cout << "Mode: reachLinear()" << endl;
    reachLinear();
    break;
  case 5:
    cout << "Mode: reachCircle()" << endl;
    reachCircle();
    break;
  case 6:
    cout << "Mode: multiTask()" << endl;
    multiTask();
    break;
//  case 3:  circle();  break;
//  case 4:  multiTask();  break;
  }

  return 0;
}
