#include <ilcplex/ilocplex.h>
#include <fstream>
#include <iostream>
#include "stdlib.h"

using namespace std;

#define MAX 1000
#define M 720
#define W 140
#define EARTH_RADIUS 6378137	  //����뾶
#define PI 3.14159265358979323846 //Բ����
#define speed 250				  //���Ա�ٶ�
#define NET_NUM 5
#define SHOP_NUM 8
#define DELV_NUM 40
#define ECO_NUM 5
#define OTO_NUM 5
#define CAR_NUM 2

ILOSTLBEGIN

class Position
{
public:
	double lng, lat;
	Position();
};
class Package
{
public:
	char id[6];
	int weight, start_time, end_time;
	Position origin, dest;
	Package();
	//����ð����ķ���ʱ��
	int service_time();
};
Position netpoint[NET_NUM + 1];
Position shop[SHOP_NUM + 1];
Position delivery[DELV_NUM + 1];
Package eco_order[ECO_NUM + 1];
Package oto_order[OTO_NUM + 1];
typedef IloArray<IloNumVarArray> NumVarMatrix;
typedef IloArray<NumVarMatrix> NumVar3Matrix;

int char_to_time(char *a)
{
	int time, hour, minute;
	hour = (a[0] - 48) * 10 + a[1] - 48;
	minute = (a[3] - 48) * 10 + a[4] - 48;
	time = (hour - 8) * 60 + minute;
	return time;
}

int char_to_int(char *a)
{
	int i = 1, id = 0;
	while (a[i] != '\0')
	{
		id = id * 10 + (a[i] - 48);
		i++;
	}
	return id;
}

double rad(double d)
{
	return d * PI / 180.0;
}

double get_distance(Position pos1, Position pos2)
{
	double lat1 = pos1.lat, lat2 = pos2.lat, lng1 = pos1.lng, lng2 = pos2.lng;
	double radLat1 = rad(lat1);
	double radLat2 = rad(lat2);
	double a = radLat1 - radLat2;
	double b = rad(lng1) - rad(lng2);

	double dst = 2 * asin((sqrt(pow(sin(a / 2), 2) + cos(radLat1) * cos(radLat2) * pow(sin(b / 2), 2))));

	dst = dst * EARTH_RADIUS;
	dst = round(dst * 10000) / 10000;
	return dst;
}

int get_travel_time(double dis)
{
	return round(dis / speed);
}
void netpoint_input()
{
	int count = 0;
	FILE *fp = fopen("data/new_1.csv", "r");
	while (count < 5)
	{
		char idchar[10], c;
		int i = 0, id = 0;
		while (1)
		{
			fscanf(fp, "%c", &idchar[i]);
			if (idchar[i] == ',')
			{
				idchar[i] = '\0';
				break;
			}
			i++;
		}
		id = char_to_int(idchar);
		fscanf(fp, "%lf", &netpoint[id].lng);
		fscanf(fp, "%c", &c);
		fscanf(fp, "%lf", &netpoint[id].lat);
		fscanf(fp, "%c\n", &c);
		count++;
	}
	fclose(fp);
}

void delivery_input()
{
	int count = 0;
	FILE *fp = fopen("data/new_2.csv", "r");
	while (count < DELV_NUM)
	{
		char idchar[10], c;
		int i = 0, id = 0;
		while (1)
		{
			fscanf(fp, "%c", &idchar[i]);
			if (idchar[i] == ',')
			{
				idchar[i] = '\0';
				break;
			}
			i++;
		}
		id = char_to_int(idchar);
		fscanf(fp, "%lf", &delivery[id].lng);
		fscanf(fp, "%c", &c);
		fscanf(fp, "%lf", &delivery[id].lat);
		fscanf(fp, "%c\n", &c);
		count++;
	}
	fclose(fp);
}

void shop_input()
{
	int count = 0;
	FILE *fp = fopen("data/new_3.csv", "r");
	while (count < SHOP_NUM)
	{
		char idchar[10], c;
		int i = 0, id = 0;
		while (1)
		{
			fscanf(fp, "%c", &idchar[i]);
			if (idchar[i] == ',')
			{
				idchar[i] = '\0';
				break;
			}
			i++;
		}
		id = char_to_int(idchar);
		fscanf(fp, "%lf", &shop[id].lng);
		fscanf(fp, "%c", &c);
		fscanf(fp, "%lf", &shop[id].lat);
		fscanf(fp, "%c\n", &c);
		count++;
	}
	fclose(fp);
}

void eorder_input()
{
	int count = 0;
	FILE *fp = fopen("data/new_4.csv", "r");
	while (count < ECO_NUM)
	{
		char idchar[10];
		int i = 0, id = 0, orderid = 0;
		while (1)
		{
			fscanf(fp, "%c", &idchar[i]);
			if (idchar[i] == ',')
			{
				idchar[i] = '\0';
				break;
			}
			i++;
		}
		orderid = char_to_int(idchar);
		strcpy(eco_order[orderid].id, idchar);
		i = 0;
		while (1)
		{
			fscanf(fp, "%c", &idchar[i]);
			if (idchar[i] == ',')
			{
				idchar[i] = '\0';
				break;
			}
			i++;
		}
		id = char_to_int(idchar);
		eco_order[orderid].dest = delivery[id];
		i = 0;
		while (1)
		{
			fscanf(fp, "%c", &idchar[i]);
			if (idchar[i] == ',')
			{
				idchar[i] = '\0';
				break;
			}
			i++;
		}
		id = char_to_int(idchar);
		eco_order[orderid].origin = netpoint[id];
		fscanf(fp, "%d\n", &eco_order[orderid].weight);
		count++;
	}
	fclose(fp);
}

void otoorder_input()
{
	int count = 0;
	FILE *fp = fopen("data/new_5.csv", "r");
	while (count < OTO_NUM)
	{
		char idchar[10];
		int i = 0, id = 0, orderid = 0;
		while (1)
		{
			fscanf(fp, "%c", &idchar[i]);
			if (idchar[i] == ',')
			{
				idchar[i] = '\0';
				break;
			}
			i++;
		}
		orderid = char_to_int(idchar);
		strcpy(oto_order[orderid].id, idchar);
		i = 0;
		while (1)
		{
			fscanf(fp, "%c", &idchar[i]);
			if (idchar[i] == ',')
			{
				idchar[i] = '\0';
				break;
			}
			i++;
		}
		id = char_to_int(idchar);
		oto_order[orderid].dest = delivery[id];
		i = 0;
		while (1)
		{
			fscanf(fp, "%c", &idchar[i]);
			if (idchar[i] == ',')
			{
				idchar[i] = '\0';
				break;
			}
			i++;
		}
		id = char_to_int(idchar);
		oto_order[orderid].origin = shop[id];
		i = 0;
		while (1)
		{
			fscanf(fp, "%c", &idchar[i]);
			if (idchar[i] == ',')
			{
				idchar[i] = '\0';
				break;
			}
			i++;
		}
		oto_order[orderid].start_time = char_to_time(idchar);
		i = 0;
		while (1)
		{
			fscanf(fp, "%c", &idchar[i]);
			if (idchar[i] == ',')
			{
				idchar[i] = '\0';
				break;
			}
			i++;
		}
		oto_order[orderid].end_time = char_to_time(idchar);
		fscanf(fp, "%d\n", &oto_order[orderid].weight);
		count++;
	}
	fclose(fp);
}

IloInt depot_index, depot_num, Eo_index, Ed_index, Fo_index, Fd_index, courior, order_num, end_index, node_num;
IloNumArray e, l, q, s;
IloNumArray2 t;

Position::Position()
{
	lng = 0;
	lat = 0;
}
Package::Package()
{
	origin = Position();
	dest = Position();
	weight = 0;
	start_time = 0;
	end_time = 720;
}

int Package::service_time()
{
	return round(3 * sqrt(weight) + 5);
}

void define_data(IloEnv env)
{
	depot_num = NET_NUM;
	courior = CAR_NUM;
	order_num = OTO_NUM + ECO_NUM;
	depot_index = 0;
	Eo_index = depot_index + depot_num;
	Fo_index = Eo_index + OTO_NUM;
	Ed_index = Fo_index + ECO_NUM;
	Fd_index = Ed_index + OTO_NUM;
	end_index = Fd_index + ECO_NUM;
	node_num = end_index + 1;
	t = IloNumArray2(env, node_num);
	e = IloNumArray(env, node_num);
	l = IloNumArray(env, node_num);
	q = IloNumArray(env, node_num);
	s = IloNumArray(env, node_num);
	for (int i = depot_index; i < node_num; i++)
	{
		Position iPos, jPos;
		t[i] = IloNumArray(env, node_num);
		if (i < Eo_index)
		{
			iPos = netpoint[i + 1];
			e[i] = IloInt(0);
			l[i] = IloInt(720);
			q[i] = IloInt(0);
			s[i] = IloInt(0);
		}
		else if (i < Fo_index)
		{
			iPos = oto_order[i - Eo_index + 1].origin;
			e[i] = IloInt(oto_order[i - Eo_index + 1].start_time);
			l[i] = IloInt(oto_order[i - Eo_index + 1].end_time);
			q[i] = IloInt(oto_order[i - Eo_index + 1].weight);
			s[i] = IloInt(0);
		}
		else if (i < Ed_index)
		{
			iPos = eco_order[i - Fo_index + 1].origin;
			e[i] = IloInt(eco_order[i - Fo_index + 1].start_time);
			l[i] = IloInt(eco_order[i - Fo_index + 1].end_time);
			q[i] = IloInt(eco_order[i - Fo_index + 1].weight);
			s[i] = IloInt(0);
		}
		else if (i < Fd_index)
		{
			iPos = oto_order[i - Ed_index + 1].dest;
			e[i] = IloInt(oto_order[i - Ed_index + 1].start_time);
			l[i] = IloInt(oto_order[i - Ed_index + 1].end_time);
			q[i] = IloInt(-oto_order[i - Ed_index + 1].weight);
			s[i] = IloInt(oto_order[i - Ed_index + 1].service_time());
		}
		else if (i < end_index)
		{
			iPos = eco_order[i - Fd_index + 1].dest;
			e[i] = IloInt(eco_order[i - Fd_index + 1].start_time);
			l[i] = IloInt(eco_order[i - Fd_index + 1].end_time);
			q[i] = IloInt(-eco_order[i - Fd_index + 1].weight);
			s[i] = IloInt(eco_order[i - Fd_index + 1].service_time());
		}
		else
		{
			e[i] = IloInt(0);
			l[i] = IloInt(720);
			q[i] = IloInt(0);
			s[i] = IloInt(0);
		}
		for (int j = depot_index; j < node_num; j++)
		{
			if (i == j || i == end_index)
			{
				t[i][j] = IloInt(MAX);
				continue;
			}
			if (j < Eo_index)
			{
				jPos = netpoint[j + 1];
			}
			else if (j < Fo_index)
			{
				jPos = oto_order[j - Eo_index + 1].origin;
			}
			else if (j < Ed_index)
			{
				jPos = eco_order[j - Fo_index + 1].origin;
			}
			else if (j < Fd_index)
			{
				jPos = oto_order[j - Ed_index + 1].dest;
			}
			else if (j < end_index)
			{
				jPos = eco_order[j - Fd_index + 1].dest;
			}
			double dis = get_distance(iPos, jPos);
			if (j != end_index)
				t[i][j] = IloInt(get_travel_time(dis));
			else
				t[i][j] = IloInt(0);
		}
	}
}

ILOMIPINFOCALLBACK6(timeIntervalCallback,
					IloCplex, cplex,
					ofstream &, fp,
					IloNum, timeStart,
					IloNum, timeInterval,
					IloNum, timeBefore,
					IloNum, incumbent)
{
	int better_flag = 0;
	IloNum timeNow = cplex.getCplexTime() - timeStart;
	if (hasIncumbent())
	{
		if (incumbent != getIncumbentObjValue())
		{
			better_flag = 1;
			incumbent = getIncumbentObjValue();
		}
		if (timeNow - timeBefore > timeInterval || better_flag)
		{
			better_flag = 0;
			fp << timeNow << "," << getIncumbentObjValue() << endl;
			timeBefore = timeNow;
		}
	}
	if (timeNow / 3600 > 5)
		abort();
}

int main()
{
	netpoint_input();
	delivery_input();
	shop_input();
	eorder_input();
	otoorder_input();
	IloEnv env;
	//ofstream res("CallBackResult.csv", ios::app);
	try
	{
		define_data(env);

		//check input data
		/*for (int i = 0; i < node_num; i++) {
			for (int j = 0; j < node_num; j++) {
				cout << t[i][j] << " ";
			}
			cout << endl;
		}
		cout << "time window:";
		for (int i = 0; i < node_num; i++) {
			cout << e[i] << "/" << l[i] << "  ";
		}
		cout << endl << "weight/service:";
		for (int i = 0; i < node_num; i++) {
			cout << q[i] << "/" << s[i] << "  ";
		}*/

		IloModel model(env);

		//Variable Define
		NumVar3Matrix x(env, courior);
		for (int k = 0; k < courior; k++)
		{
			x[k] = NumVarMatrix(env, node_num);
			for (int i = 0; i < node_num; i++)
			{
				x[k][i] = IloNumVarArray(env, node_num);
				for (int j = 0; j < node_num; j++)
				{
					x[k][i][j] = IloNumVar(env, 0, 1, ILOINT);
				}
			}
		}
		IloNumVarArray a(env, node_num);
		IloNumVarArray b(env, node_num);
		IloNumVarArray dt(env, node_num);
		IloNumVarArray w(env, node_num);
		IloNumVarArray wt(env, node_num);
		IloNumVarArray at(env, node_num);
		for (int i = 0; i < node_num; i++)
		{
			dt[i] = IloNumVar(env, 0, IloInfinity, ILOINT);
			w[i] = IloNumVar(env, 0, IloInfinity, ILOINT);
			wt[i] = IloNumVar(env, 0, IloInfinity, ILOINT);
			at[i] = IloNumVar(env, 0, IloInfinity, ILOINT);
		}

		// Objective Function
		IloExpr travel_time(env);
		IloExpr penalty(env);
		IloExpr other_time(env);
		for (int k = 0; k < courior; k++)
		{
			for (int i = depot_index; i < node_num; i++)
			{
				for (int j = depot_index; j < node_num; j++)
				{
					travel_time += t[i][j] * x[k][i][j];
				}
			}
		}
		for (int i = depot_index; i < node_num; i++)
		{
			other_time += s[i] + wt[i];
		}
		for (int i = Eo_index; i < Fo_index; i++)
		{
			penalty += IloMax(0, at[i] - e[i]);
			penalty += IloMax(0, at[i + order_num] - l[i + order_num]);
		}
		model.add(IloMinimize(env, travel_time + other_time + 5 * penalty));
		travel_time.end();
		other_time.end();
		penalty.end();

		// Constraints

		//every courior start from depot and finish delivery at end node
		for (int k = 0; k < courior; k++)
		{
			IloExpr depotStart(env);
			for (int i = depot_index; i < Eo_index; i++)
			{
				for (int j = Eo_index; j < node_num; j++)
				{
					depotStart += x[k][i][j];
				}
			}
			model.add(depotStart == 1);
			depotStart.end();
		}
		for (int k = 0; k < courior; k++)
		{
			IloExpr endVehicle(env);
			for (int i = depot_index; i < end_index; i++)
			{
				endVehicle += x[k][i][end_index];
			}
			model.add(endVehicle == 1);
			endVehicle.end();
		}

		//every order have to be visited and only be visited once
		for (int j = Eo_index; j < end_index; j++)
		{
			IloExpr orderVisit(env);
			for (int k = 0; k < courior; k++)
			{
				for (int i = depot_index; i < node_num; i++)
				{
					if (i == j)
						continue;
					orderVisit += x[k][i][j];
				}
			}
			model.add(orderVisit == 1);
			orderVisit.end();
		}

		//flow constraint
		for (int k = 0; k < courior; k++)
		{
			for (int i = Eo_index; i < end_index; i++)
			{
				IloExpr originInFlow(env);
				IloExpr originOutFlow(env);
				for (int j = depot_index; j < node_num; j++)
				{
					if (i == j)
						continue;
					originInFlow += x[k][j][i];
				}
				for (int j = depot_index; j < node_num; j++)
				{
					if (i == j)
						continue;
					originOutFlow += x[k][i][j];
				}
				model.add(originInFlow == originOutFlow);
				originInFlow.end();
				originOutFlow.end();
			}
		}

		//orders have to be pickup and delivered by same courior
		for (int k = 0; k < courior; k++)
		{
			for (int j = Eo_index; j < Ed_index; j++)
			{
				IloExpr origin(env);
				IloExpr dest(env);
				for (int i = depot_index; i < node_num; i++)
				{
					if (i == j)
						continue;
					origin += x[k][j][i];
				}
				for (int i = depot_index; i < node_num; i++)
				{
					if (i == j + order_num)
						continue;
					dest += x[k][i][j + order_num];
				}
				model.add(origin == dest);
				origin.end();
				dest.end();
			}
		}

		//contiunity of arrival time for every node
		for (int i = depot_index; i < end_index; i++)
		{
			for (int j = depot_index; j < end_index; j++)
			{
				if (i == j)
					continue;
				IloExpr arcVisit(env);
				for (int k = 0; k < courior; k++)
				{
					arcVisit += x[k][i][j];
				}
				model.add(at[j] >= dt[i] + t[i][j] - MAX * (1 - arcVisit));
				arcVisit.end();
			}
		}
		for (int i = depot_index; i < end_index; i++)
		{
			for (int j = depot_index; j < end_index; j++)
			{
				if (i == j)
					continue;
				IloExpr arcVisit(env);
				for (int k = 0; k < courior; k++)
				{
					arcVisit += x[k][i][j];
				}
				model.add(at[j] <= dt[i] + t[i][j] + MAX * (1 - arcVisit));
				arcVisit.end();
			}
		}

		//waiting time constraints
		for (int i = Eo_index; i < Fo_index; i++)
		{
			model.add(wt[i] >= e[i] - at[i]);
			model.add(wt[i + order_num] >= e[i + order_num] - at[i + order_num]);
		}
		for (int i = Eo_index; i < Ed_index; i++)
		{
			if (i < Fo_index)
			{
				model.add(wt[i] >= 0);
				model.add(wt[i + order_num] >= 0);
			}
			else
			{
				model.add(wt[i] == 0);
				model.add(wt[i + order_num] == 0);
			}
		}

		//departure time constraints
		for (int i = depot_index; i < node_num; i++)
		{
			model.add(dt[i] == at[i] + wt[i] + s[i]);
		}

		//order must be pickup first, then deliver
		for (int i = Eo_index; i < Ed_index; i++)
		{
			model.add(dt[i] <= dt[i + order_num]);
		}

		//every vehicle start delivery by 8 am.
		for (int i = depot_index; i < Eo_index; i++)
		{
			model.add(dt[i] == 0);
		}

		//open time window constriant & work time constriant
		for (int i = depot_index; i < node_num; i++)
		{
			model.add(dt[i] - s[i] >= e[i]);
			model.add(dt[i] <= M);
		}

		//capacity continuity
		for (int i = depot_index; i < end_index; i++)
		{
			for (int j = Eo_index; j < end_index; j++)
			{
				IloExpr arcVisit(env);
				for (int k = 0; k < courior; k++)
				{
					arcVisit += x[k][i][j];
				}
				model.add(w[j] >= w[i] + q[j] - MAX * (1 - arcVisit));
				arcVisit.end();
			}
		}

		//depot load constraint
		for (int i = 0; i < depot_index; i++)
			model.add(w[i] == 0);

		//capacity constriant
		for (int i = 0; i < node_num; i++)
		{
			model.add(w[i] >= 0);
			model.add(w[i] <= W);
		}

		//binary variable constraint
		for (int k = 0; k < courior; k++)
		{
			for (int i = 0; i < node_num; i++)
			{
				for (int j = 0; j < node_num; j++)
				{
					model.add(x[k][i][j] >= 0);
					model.add(x[k][i][j] <= 1);
				}
			}
		}

		// Optimize
		IloCplex cplex(model);
		//Paremeter setting
		cplex.setParam(IloCplex::Param::MIP::Strategy::File, 3);
		cplex.setParam(IloCplex::Param::WorkDir, "/home/linfei");
		//CallBack
		//cplex.use(timeIntervalCallback(env, cplex, res, cplex.getCplexTime(), 5.0, 0, INFINITY));
		cplex.solve();
		//cplex.exportModel("best_route.lp");
		env.out() << "Solution status = " << cplex.getStatus() << endl;
		env.out() << "Solution value  = " << cplex.getObjValue() << endl;
		//display the value of variable
		//ofstream solution("solution.csv");
		/*for (int k = 0; k < courior; k++)
		{
			int flag = 0;
			printf("courior No.%d\n", k + 1);
			// x matrix
			for (int i = depot_index; i < node_num; i++) {
				for (int j = depot_index; j < node_num; j++) {
					env.out() << cplex.getValue(x[k][i][j]) << " ";
				}
				env.out() << endl;
			}
			//vehicle route display
			for (int i = 0; i < node_num; i++)
			{
				if (i < Eo_index)
					flag = 1;
				else
					flag = 0;
				for (int j = 0; j < node_num; j++)
				{
					if (cplex.getValue(x[k][i][j]))
					{
						flag = 1;
						if (i < Eo_index)
							printf("A%d", i + 1);
						if (j < Fo_index)
						{
							printf(" -> Eo%d", j - Eo_index + 1);
							solution << "D000" << k + 1 << ","
									 << "E000" << j - Eo_index + 1 << "," << cplex.getValue(at[j])
									 << "," << cplex.getValue(dt[j]) << "," << e[j] << "," << l[j] << endl;
						}
						else if (j < Ed_index)
						{
							printf(" -> Fo%d", j - Fo_index + 1);
							solution << "D000" << k + 1 << ","
									 << "F000" << j - Fo_index + 1 << "," << cplex.getValue(at[j])
									 << "," << cplex.getValue(dt[j]) << "," << e[j] << "," << l[j] << endl;
						}
						else if (j < Fd_index)
						{
							printf(" -> Ed%d", j - Ed_index + 1);
							solution << "D000" << k + 1 << ","
									 << "E000" << j - Ed_index + 1 << "," << cplex.getValue(at[j])
									 << "," << cplex.getValue(dt[j]) << "," << e[j] << "," << l[j] << endl;
						}
						else if (j < end_index)
						{
							printf(" -> Fd%d", j - Fd_index + 1);
							solution << "D000" << k + 1 << ","
									 << "F000" << j - Fd_index + 1 << "," << cplex.getValue(at[j])
									 << "," << cplex.getValue(dt[j]) << "," << e[j] << "," << l[j] << endl;
						}
						else
						{
							printf(" -> end");
						}
						i = j - 1;
						break;
					}
				}
				if (flag == 0)
					break;
			}
			printf("\n");
		}
		solution.close();*/

		printf("arrival time:");
		for (int i = Eo_index; i < node_num; i++)
		{
			env.out() << cplex.getValue(at[i]) << " ";
		}

		printf("\nwaiting time:");
		for (int i = Eo_index; i < node_num; i++)
		{
			env.out() << cplex.getValue(wt[i]) << " ";
		}

		printf("\ndeparture time:");
		for (int i = depot_index; i < node_num; i++)
		{
			env.out() << cplex.getValue(dt[i]) << " ";
		}

		printf("\nopen penalty:");
		for (int i = Eo_index; i < Fo_index; i++)
		{
			env.out() << cplex.getValue(IloMax(0, at[i] - e[i])) << " ";
		}

		printf("\nclose penalty:");
		for (int i = Eo_index; i < Fo_index; i++)
		{
			env.out() << cplex.getValue(IloMax(0, at[i + order_num] - l[i + order_num])) << " ";
		}

		cout << endl;
	}
	catch (IloException &ex)
	{
		cerr << "Error: " << ex << endl;
	}
	catch (...)
	{
		cerr << "Error" << endl;
	}
	//res.close();
	env.end();
	return 0;
}
