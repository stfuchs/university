"dummy"

function res = P(idx, a=0, deps=[])
  network = [
             0.02 % P(B=bad)
             0.05 % P(F=empty)
             0.99 % P(G=empty | B=bad, F=empty) 00
             0.97 % P(G=empty | B=good, F=empty) 01
             0.10 % P(G=empty | B=bad, F=not empty) 10
             0.04 % P(G=empty | B=good, F=not empty) 11
             0.98 % P(T=no | B=bad)
             0.03 % P(T=no | B=good)
             1.00 % P(S=no | T=no, F=empty)
             0.92 % P(S=no | T=yes, F=empty)
             1.00 % P(S=no | T=no, F=not empty)
             0.01 % P(S=no | T=yes, F=not empty)
             ];

  offset = [ 1 1 4 2 4 ];

  if(length(deps) > offset(idx)/2) 
    printf("provided dependencies do not fit\n");
    return;
  end

  off = 1;
  for i=1:idx-1
    off += offset(i);
  end
  for i=1:length(deps)
    off += 2^(i-1)*deps(i);
  end

  if(a==1) res = 1.0 - network(off);
  else res = network(off);
  end
endfunction

prop = zeros(32,1);
name = cell(32,2);
idx = 1;
for s=[0 1]
  for t=[0 1]
    for g=[0 1]
      for f=[0 1]
        for b=[0 1]
          prop(idx) = P(1,b) * P(2,f) * P(3,g,[b,f]) * P(4,t,[b]) * P(5,s,[t,f]);
          name{idx} = dec2bin(idx-1,5);
          idx++;
        end
      end
    end
  end
end

num = zeros(2,2);
nom(1)
s_1 = num(:,2) / denom(2)