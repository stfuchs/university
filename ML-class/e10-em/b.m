printf("Expectation Maximization\n")
printf("Gaussian Mixture Models\n")
clear;

function drawGaussian(mu, cov, props)
[U, D, V] = svd(cov);
lamba = diag(D);

mu1 = mu + sqrt(lamba(1)) * V(1,:);
mu2 = mu + sqrt(lamba(2)) * V(2,:);

hold on
plot(mu(1), mu(2), 'ko')
plot([mu(1) mu1(1)], [mu(2) mu1(2)], props)
plot([mu(1) mu2(1)], [mu(2) mu2(2)], props)
hold off
end

function P = estep(X, cov, mu, K)
  n = length(X);
  d = size(X)(2);

  for k=1:K
    frac(k) = 1.0 / (sqrt( det(2.0*pi*cov(:,:,k)) ) * K );
    cov_inv(:,:,k) = inv(cov(:,:,k));
  end

  for i=1:n
    for k=1:K
      P(k,i) = frac(k) * exp( -0.5 * (X(i,:) - mu(k,:)) * cov_inv(:,:,k) * transpose(X(i,:) - mu(k,:)) );
    end
    P_x(i) = sum(P(:,i));
    P(:,i) /= P_x(i);
  end
end

function [cov, mu] = mstep(X, P, K)
  n = length(X);
  d = size(X)(2);

  for k=1:K
    mu(k,:) = zeros(1,d);
    cov(:,:,k) = zeros(d);
    denom(k) = 1.0 / sum(P(k,:));
    for i=1:n
      mu(k,:) += P(k,i) * X(i,:);
    end
    mu(k,:) *= denom(k);

    for i=1:n
      demean = X(i,:) - mu(k,:);
      cov(:,:,k) += P(k,i) * transpose(demean) * demean;
    end
    cov(:,:,k) *= denom(k);
  end
end

function [cov, mu] = init(X, K, props)
  n = length(X);
  d = size(X)(2);

  if(strcmp(props, "rand label"))
    for i=1:n
      for k=1:K
        q(k,i) = rand(1);
      end
      q(:,i) /= sum(q(:,i));
    end
    [cov, mu] = mstep(X, q, K);

  elseif(strcmp(props, "rand mean"))
    for k=1:K
      mu(k,:) = X(round(rand(1)*n),:);
      cov(:,:,k) = diag(ones(d,1));
    end
  end
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

X = load('mixture.txt');
L = 20;

%[C, mu] = init(X, 3, 'rand mean'); % a)
[C, mu] = init(X, 3, 'rand label'); % b)

likelihood_prev = 0.0;
for l=1:L

  q = estep(X, C, mu, 3);
  [C, mu] = mstep(X, q, 3);
end

clf
plot(X(:,1), X(:,2), 'k+')
drawGaussian(mu(1,:), C(:,:,1), 'r');
drawGaussian(mu(2,:), C(:,:,2), 'g');
drawGaussian(mu(3,:), C(:,:,3), 'b');
axis('square')
