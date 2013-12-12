function rmax()
  %threshold to stop
  threshold = 0.01;
  m = 100; % 40, 60, 80, 100
  epsilon = 0.001;

  % learning param
  alpha = 0.2;
  beta = 0.1;
  gamma = 0.9;
  % environment setting
  n_states = 12;
  n_actions = 4;

  n1 = zeros(12,4);   % store counts of n(s,a)
  n2 = zeros(12,4,12); % store counts of n(s,a,s')
  r1 = zeros(12,4);

  % initialize T and R
  % moves[12][4]: the next state if taking an action (true move without noise):
  % 7 and 16 are terminal states
  moves = [1 2 4 1;
           1 3 5 2;
           2 3 6 3;
           4 5 7 1;
           4 6 8 2;
           5 6 9 3;
           7 8 10 4;
           8 8 8 8;
           8 9 12 6;
           10 11 10 9;
           10 12 11 8;
           12 12 12 12];

  rmax = 1;
  T = ones(12,4,12) * 1/12;    % transition propabilities
  T_prev = zeros(12,4,12);
  %for s=1:n_states
  %  for a=1:n_actions
  %      % initialize with noise free action
  %      T(s,a,moves(s,a)) = 1;%/n_actions;
  %    %end
  %  end
  %end
  R = ones(12,4) * rmax;


  % initial policy
  %
  state = 1;
  diff = 1;
  iteration = 0;
  [policy,v_new] = ValueIteration(T,R,gamma,epsilon);
  while (iteration < m*60) %diff > threshold) && (iteration < 100)
    iteration = iteration + 1;
    % taking action
    % disp(policy')
    action = policy(state);
    [sNext,reward,term] = simulator(action, state);
    % check if terminal state, then reset state = 1 (inital state)

    n1(state,action) = n1(state,action) + 1;
    n2(state,action,sNext) = n2(state,action,sNext) + 1;
    r1(state,action) = r1(state,action) + reward;

    if( n1(state,action) == m )
      denom = 1.0 / n1(state,action);
      state;
      action;
      %diff = 0;
      for i=1:n_states
        update = n2(state,action,i) * denom;
        %diff = diff + (update - T(state,action,i))^2;
        T(state,action,i) = update;
      end
      R(state,action) = r1(state,action) * denom;
      [policy,v_new] = ValueIteration(T,R,gamma,epsilon);
      %disp(R)
      %disp(v_new')
      %iff = max(T_prev-T);
      %T_prev = T;
      disp(iteration)
      %if diff < threshold
      %   disp('break')
      %   break
      %end
    end


    %if n1(state,action) > 5*m
    %   break
    %end

    if term
      state = 1;
    else
      state = sNext;
    end
    %===== RMAX code
  end

  %for s=1:12
  %  sa = zeros(4,12);
  %  for a=1:4
  %    for ss=1:12
  %        sa(a,ss) = T(s,a,ss);
  %    end
  %  end
  %  disp(sa)
  %end
  disp(R)
  disp(n1)
  disp(policy')
  disp(v_new')
end

% Return a policy of the given MDP = (T,R,gamma) with accuracy epsilon
function [policy,v_new] = ValueIteration(T,R,gamma,epsilon)
  policy = ones(12,1);
  v_old = zeros(12,1);
  v_new = zeros(12,1);
  diff = 1.0;
  iteration = 0;
  while (diff > epsilon)% && iteration < 1000)
    for s=1:12
      r_max = -100000;
      for a=1:4
        r_curr = 0;
        for j=1:12
          r_curr = r_curr + v_old(j) * T(s,a,j);
        end
        r_curr = R(s,a) + gamma * r_curr;
        if (r_curr > r_max)
           r_max = r_curr;
           policy(s) = a;
        end
      end
      v_new(s) = r_max;
      %disp(r_max)
    end
    diff = max(v_new - v_old);
    iteration = iteration + 1;
    v_old = v_new;
    %disp(v_old')
  end
  %===== Value Iteration code
end

function [sNext,reward, term] = simulator(action, state)
  % the environment is 4x3 maze
  % noisy movement: 0.8 to intended direction, 0.1 each to 2 perpendicular directions
  % action: 1: left; 2: right; 3: down; 4: up
    % 1(start) 2        3
  % 4        5        6
  % 7        8(cliff) 9
  % 10       11       12  (goal)


  n_states = 12;
  n_actions = 4;
  noisy = 0.2;

   % noisy[taken_act][noisy_act] : 0.8 is noisy[taken_act][1]; 0.1 noisy[taken_act][2] and noisy[taken_act][3]
  noisyAct = [1, 3, 4; 2, 3, 4; 3, 1, 2; 4, 1, 2];

  % moves[12][4]: the next state if taking an action (true move without noise): 7 and 16 are terminal states
  moves = [1 2 4 1; 1 3 5 2; 2 3 6 3; 4 5 7 1; 4 6 8 2; 5 6 9 3; 7 8 10 4; 8 8 8 8; 8 9 12 6;10 11 10 9; 10 12 11 8; 12 12 12 12];



  % if not terminal
  term = 0;
  reward = -1.0; %movement cost

  %sample next state
  prob = rand(1);
  true_act = noisyAct(action,1);
  noise_act_1 = noisyAct(action,2);
  noise_act_2 = noisyAct(action,3);

  if (state == 5)
    if (prob < 0.8)
      sNext = moves(state,true_act);
    elseif (prob < 0.9)
      sNext = moves(state,noise_act_1);
    else
      sNext = moves(state,noise_act_2);
    end
  else
    % other states have deterministic movement
    sNext = moves(state,true_act);
  end


   % check terminal at trap state
  if (sNext == 8)
    sNext = 8;
    reward = -10;
    term = 1;
    return
  end

  % check terminal at goal state
  if (sNext == 12)
    sNext = 12;
    reward = 1;
    term = 1;
    return
  end
end
