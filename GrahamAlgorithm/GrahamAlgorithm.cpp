#include "stdafx.h"
#include <iostream>
#include <stack>
#include <fstream>
#include <ostream>

struct SPoint
{
	SPoint() = default;
	SPoint(int x, int y) : x(x), y(y) {};
	int x;
	int y;
};

enum class Orientation
{
	Collinear,
	RightSwitch,
	LeftSwitch
};

class CGrahamAlgorithm 
{
public:
	CGrahamAlgorithm() = default;
	~CGrahamAlgorithm() = default;

	void InitFromFile(const std::string & fileName)
	{
		std::ifstream in(fileName);
		int countOfPoints;
		in >> countOfPoints;
		int x, y;
		for (int i = 0; i < countOfPoints; i++)
		{
			in >> x >> y;
			m_points.push_back(SPoint(x, y));
		}
	}

	void FindConvexHull()
	{
		SetLowestPoint();

		std::sort(m_points.begin() + 1, m_points.end(), 
			[this](const auto& p1, const auto& p2)
		{
			Orientation orientation = GetSwitchOrientation(m_firstPoint, p1, p2);
			if (orientation == Orientation::Collinear)
			{
				return GetDistanceBetweenPoints(m_firstPoint, p2) >= GetDistanceBetweenPoints(m_firstPoint, p1);
			}
			return orientation == Orientation::LeftSwitch;
		});

		auto countOfPoint = GetCountOfNonCollinearPoint();

		if (countOfPoint >= 3)
		{
			PushPointsToStack(countOfPoint);
		}
	}

	void OutputPointsOfConvexHull()
	{
		std::ofstream out("output.txt");
		out << m_pointsStack.size() << std::endl;
		while (!m_pointsStack.empty())
		{
			SPoint p = m_pointsStack.top();
			out << p.x << " " << p.y << std::endl;
			m_pointsStack.pop();
		}
	}

private:
	SPoint GetNextToTopPoint(std::stack<SPoint> &S)
	{
		SPoint top = S.top();
		S.pop();

		SPoint nextFromTop = S.top();
		S.push(top);

		return nextFromTop;
	}

	double GetDistanceBetweenPoints(SPoint p1, SPoint p2)
	{
		return std::hypot(p1.x - p2.x, p1.y - p2.y);
	}

	Orientation GetSwitchOrientation(const SPoint & first, const SPoint & second, const SPoint & third)
	{
		int coefOfLineAngle = (first.y - second.y) * (third.x - first.x) -
			(first.x - second.x) * (third.y - first.y);

		if (coefOfLineAngle == 0)
		{
			return Orientation::Collinear;
		}
		return (coefOfLineAngle > 0) ? Orientation::RightSwitch : Orientation::LeftSwitch;
	}

	void SetLowestPoint()
	{
		int min = 0;
		for (std::size_t i = 1; i < m_points.size(); i++)
		{
			int y = m_points[i].y;
			if ((y < m_points[min].y) || (m_points[min].y == y && m_points[i].x < m_points[min].x))
			{
				min = i;
			}
		}

		std::swap(m_points[0], m_points[min]);
		m_firstPoint = m_points[0];
	}

	std::size_t GetCountOfNonCollinearPoint()
	{
		std::size_t countOfPoint = 1;
		for (std::size_t i = 1; i < m_points.size(); i++)
		{
			while (i < m_points.size() - 1 && GetSwitchOrientation(m_firstPoint, m_points[i], m_points[i + 1]) == Orientation::Collinear)
			{
				i++;
			}

			m_points[countOfPoint] = m_points[i];
			countOfPoint++;
		}
		return countOfPoint;
	}

	void PushPointsToStack(std::size_t countOfPoints)
	{
		for (std::size_t i = 0; i < countOfPoints; i++)
		{
			if (i < 3)
			{
				m_pointsStack.push(m_points[i]);
				continue;
			}
			while (GetSwitchOrientation(GetNextToTopPoint(m_pointsStack), m_pointsStack.top(), m_points[i]) != Orientation::LeftSwitch)
			{
				m_pointsStack.pop();
			}

			m_pointsStack.push(m_points[i]);
		}
	}

	SPoint m_firstPoint;
	std::vector<SPoint> m_points;
	std::stack<SPoint> m_pointsStack;
};

int main()
{
	CGrahamAlgorithm graham;

	graham.InitFromFile("input.txt");
	graham.FindConvexHull();
	graham.OutputPointsOfConvexHull();

	return 0;
}