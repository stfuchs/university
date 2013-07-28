X = load('gauss.txt');
n = length(X);
mu_t = 1.0/n*sum(X);
Xd = X - ones(n,1) * mu_t;

C1 = 1.0/n * transpose(Xd) * Xd;
C2 = 1.0/n * transpose(X) * X - transpose(mu_t) * mu_t;

[U, D, V] = svd(C2);

lamba = diag(D)
V

mu1 = mu_t + sqrt(lamba(1)) * V(1,:);
mu2 = mu_t + sqrt(lamba(2)) * V(2,:);

clf
plot(X(:,1), X(:,2), 'r+')
hold on
plot(mu_t(1), mu_t(2), 'bo')
plot([mu_t(1) mu1(1)], [mu_t(2) mu1(2)], 'g')
plot([mu_t(1) mu2(1)], [mu_t(2) mu2(2)], 'b')
axis('square')
hold off
