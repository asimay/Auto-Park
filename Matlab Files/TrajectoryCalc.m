clear
close all
clc

% Constants
R = 1000;        % Turning radius, mm
r = 455/2;       % Robot radius, mm
dc = 50;         % margin of error to leave between obstacles, mm
vmax = 500;        % Robot default speed, mm/s
omegamax = 150*pi/180;  % Robot maximum rotational speed, rad/s
d2r = pi/180;

% Simulation environment
D = 500;        % Depth of parking spot, mm
L = 1500;       % Length of parking spot, mm
dwall = 1000;   % Initial distance of robot to wall
bx = [0; 800; 800; 1600; 1600; 1600+L; 1600+L; 2400+L];
by = [-dwall; -dwall; D-dwall; D-dwall; -dwall; -dwall; D-dwall; D-dwall];

%% Determine Parking Spot

% Inputs for determining parking spot
v_search = vmax*0.8;    % linear velocity, mm/s
omega_search = 0;     % angular velocity, rad/s
T = bx(4)/v_search;   % time, s

% Simulate searching for parking spot
psi0 = 0;
sim('Robot_Model_Search')
px_search = px_ts_search.Data;
py_search = py_ts_search.Data;
psi_search = psi_ts_search.Data;
v_search = v_ts_search.Data;
omega_search = omega_ts_search.Data;
r_search = r_ts_search.Data;
alpha_search = alpha_ts_search.Data;
t_search = px_ts_search.Time;

%% Calculate parking trajectory

% Calculate cars positions
px_car1 = px_search(end)+r_search(end,4)*cos(alpha_search(end,4));
px_car2 = px_search(end)+r_search(end,7)*cos(alpha_search(end,7));
yc = py_search(end)+r_search(end,4)*sin(alpha_search(end,4));

% Calculate distance to wall
yw = py_search(end)+r_search(end,6)*sin(alpha_search(end,6));

% Calculate desired final position
L = px_car2-px_car1;
xf = px_car1+dc+r;
D = yc-yw;
if D<2*(r+dc)
    dw = dc;
else
    dw = (D-2*r)/2;
end
yf = yw+dw+r;

% Determine if parking space is long enough
if (dc+r-L)^2+(dw+r+R-D)^2 > (R+r+dc)^2
    disp('Parking spot is long enough. Beginning parking sequence.')
else
    error('Parking spot is too short')
end

% Calculate center of circle one
xc1 = xf;
yc1 = yf+R;

% Calculate triangle angle
Sy = yf-py_search(end)+2*R;
A = asin(Sy/(2*R));
yc2 = yc1-Sy;

% Calculate x distance to move for alignment
Sx = 2*R*cos(A);
deltax = xf+Sx-px_search(end);
xc2 = px_search(end)+deltax;

%% Simulate Parking

% Back up proper distance
v_park = vmax*0.8;
omega_park = 0;
T = abs(deltax/v_park);
x0 = px_search(end);
y0 = py_search(end);
psi0 = psi_search(end);

sim('Robot_Model_Park')
px_park1 = px_ts_park.Data;
py_park1 = py_ts_park.Data;
psi_park1 = psi_ts_park.Data;
t1 = px_ts_park.Time+t_search(end);
v_park1 = v_ts_park.Data;
omega_park1 = omega_ts_park.Data;

% First circle turn
v_park = -vmax*0.8;
omega_park = -v_park/R;
T = abs(((pi/2)-A)/omega_park);
x0 = px_park1(end);
y0 = py_park1(end);
psi_park1(end);

sim('Robot_Model_Park')
px_park2 = px_ts_park.Data;
py_park2 = py_ts_park.Data;
psi_park2 = psi_ts_park.Data;
t2 = px_ts_park.Time+t1(end);
v_park2 = v_ts_park.Data;
omega_park2 = omega_ts_park.Data;

% Second circle turn
v_park = -vmax*0.8;
omega_park = v_park/R;
T = abs(((pi/2)-A)/omega_park);
x0 = px_park2(end);
y0 = py_park2(end);
psi0 = psi_park2(end);

sim('Robot_Model_Park')
px_park3 = px_ts_park.Data;
py_park3 = py_ts_park.Data;
t3 = px_ts_park.Time+t2(end);
v_park3 = v_ts_park.Data;
omega_park3 = omega_ts_park.Data;
psi_park3 = psi_ts_park.Data;

% Combine 3 steps
px_park = [px_park1; px_park2; px_park3];
py_park = [py_park1; py_park2; py_park3];
v_park = [v_park1; v_park2; v_park3];
omega_park = [omega_park1; omega_park2; omega_park3];
psi_park = [psi_park1; psi_park2; psi_park3];
t_park = [t1; t2; t3];

%% Plot results

% Create circles to plot
a = 0:0.01:2*pi;
c1x = xc1+R*cos(a);
c1y = yc1+R*sin(a);
c2x = xc2+R*cos(a);
c2y = yc2+R*sin(a);

% Trajectory plot
figure(1)
hold on
plot(bx,by,'k')
plot(px_search,py_search,'b',px_search-r*sin(psi_search),py_search+r*cos(psi_search),'b--',px_search+r*sin(psi_search),py_search-r*cos(psi_search),'b--')
plot(px_park,py_park,'r',px_park-r*sin(psi_park),py_park+r*cos(psi_park),'r--',px_park+r*sin(psi_park),py_park-r*cos(psi_park),'r--')
plot(c1x,c1y,'g')
plot(c2x,c2y,'g')
% plot([xc1,xc2,xc1,xc1],[yc1,yc2,yc2,yc1],'b--')
%%
% Input Time Histories
figure(2)
subplot(211)
plot(t_search,v_search,'b',t_park,v_park,'r')
legend('Search','Park')
xlabel('Time (s)')
ylabel('Velocity (mm/s)')
subplot(212)
plot(t_search,omega_search,'b',t_park,omega_park,'r')
xlabel('Time (s)')
ylabel('Rotational Velocity (rad/s)')

% Debugging Time Histories
figure(3)
subplot(211)
plot(alpha_ts_search*(180/pi))
xlabel('Time (s)')
ylabel('Alpha (deg)')
subplot(212)
plot(psi_ts_search)
xlabel('Time (s)')
ylabel('Heading Angle (rad)')