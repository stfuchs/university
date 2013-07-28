x = linspace(-1,1,100);
K = zeros(100,100);
for i=1:100
  for j=1:100
    K(i,j) = exp( -((x(i)-x(j))/0.2)^2.0 );
  end
end

%[xx,yy] = meshgrid(x,x);
%surf(x,x,cov)

X = [-0.5,0.5];
Y = [0.3,-0.1];

mean_cov = zeros(100);
In = diag(ones(1,100)) * 0.1^2;
for i=1:length(x)
  mean_cov(i) = K(i,i) - K(i,:) * inverse(K+In) * transpose(K(i,:));
end

for i=1:length(x)
  mean_cov2(i) = K(i,i) - K(i,:) * inverse(K+In) * transpose(K(i,:)) + 0.1^2;
end

