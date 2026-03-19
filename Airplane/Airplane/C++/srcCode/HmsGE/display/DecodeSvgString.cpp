#include "DecodeSvgString.h"
#include <string>
#include <sstream>

NS_HMS_BEGIN

CDecodeSvgString::CDecodeSvgString()
{
}


CDecodeSvgString::~CDecodeSvgString()
{

}

#define Q_PI 3.14159265358979323846

static void pathArcSegment(CHmsPainterPath &path,
	double xc, double yc,
	double th0, double th1,
	double rx, double ry, double xAxisRotation)
{
	double sinTh, cosTh;
	double a00, a01, a10, a11;
	double x1, y1, x2, y2, x3, y3;
	double t;
	double thHalf;

	sinTh = sin(xAxisRotation * (Q_PI / 180.0));
	cosTh = cos(xAxisRotation * (Q_PI / 180.0));

	a00 = cosTh * rx;
	a01 = -sinTh * ry;
	a10 = sinTh * rx;
	a11 = cosTh * ry;

	thHalf = 0.5 * (th1 - th0);
	t = (8.0 / 3.0) * sin(thHalf * 0.5) * sin(thHalf * 0.5) / sin(thHalf);
	x1 = xc + cos(th0) - t * sin(th0);
	y1 = yc + sin(th0) + t * cos(th0);
	x3 = xc + cos(th1);
	y3 = yc + sin(th1);
	x2 = x3 + t * sin(th1);
	y2 = y3 - t * cos(th1);

	path.cubicTo(a00 * x1 + a01 * y1, a10 * x1 + a11 * y1,
		a00 * x2 + a01 * y2, a10 * x2 + a11 * y2,
		a00 * x3 + a01 * y3, a10 * x3 + a11 * y3);
}

static void pathArc(CHmsPainterPath &path,
	double               rx,
	double               ry,
	double               x_axis_rotation,
	int         large_arc_flag,
	int         sweep_flag,
	double               x,
	double               y,
	double curx, double cury)
{
	double sin_th, cos_th;
	double a00, a01, a10, a11;
	double x0, y0, x1, y1, xc, yc;
	double d, sfactor, sfactor_sq;
	double th0, th1, th_arc;
	int i, n_segs;
	double dx, dy, dx1, dy1, Pr1, Pr2, Px, Py, check;

	rx = abs(rx);
	ry = abs(ry);

	sin_th = sin(x_axis_rotation * (Q_PI / 180.0));
	cos_th = cos(x_axis_rotation * (Q_PI / 180.0));

	dx = (curx - x) / 2.0;
	dy = (cury - y) / 2.0;
	dx1 = cos_th * dx + sin_th * dy;
	dy1 = -sin_th * dx + cos_th * dy;
	Pr1 = rx * rx;
	Pr2 = ry * ry;
	Px = dx1 * dx1;
	Py = dy1 * dy1;
	/* Spec : check if radii are large enough */
	check = Px / Pr1 + Py / Pr2;
	if (check > 1) {
		rx = rx * sqrt(check);
		ry = ry * sqrt(check);
	}

	a00 = cos_th / rx;
	a01 = sin_th / rx;
	a10 = -sin_th / ry;
	a11 = cos_th / ry;
	x0 = a00 * curx + a01 * cury;
	y0 = a10 * curx + a11 * cury;
	x1 = a00 * x + a01 * y;
	y1 = a10 * x + a11 * y;
	/* (x0, y0) is current point in transformed coordinate space.
	(x1, y1) is new point in transformed coordinate space.

	The arc fits a unit-radius circle in this space.
	*/
	d = (x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0);
	sfactor_sq = 1.0 / d - 0.25;
	if (sfactor_sq < 0) sfactor_sq = 0;
	sfactor = sqrt(sfactor_sq);
	if (sweep_flag == large_arc_flag) sfactor = -sfactor;
	xc = 0.5 * (x0 + x1) - sfactor * (y1 - y0);
	yc = 0.5 * (y0 + y1) + sfactor * (x1 - x0);
	/* (xc, yc) is center of the circle. */

	th0 = atan2(y0 - yc, x0 - xc);
	th1 = atan2(y1 - yc, x1 - xc);

	th_arc = th1 - th0;
	if (th_arc < 0 && sweep_flag)
		th_arc += 2 * Q_PI;
	else if (th_arc > 0 && !sweep_flag)
		th_arc -= 2 * Q_PI;

	n_segs = ceil(abs(th_arc / (Q_PI * 0.5 + 0.001)));

	for (i = 0; i < n_segs; i++) {
		pathArcSegment(path, xc, yc,
			th0 + i * th_arc / n_segs,
			th0 + (i + 1) * th_arc / n_segs,
			rx, ry, x_axis_rotation);
	}
}

// '0' is 0x30 and '9' is 0x39
static inline bool isDigit(unsigned short ch)
{
	static unsigned short magic = 0x3ff;
	return ((ch >> 4) == 3) && (magic >> (ch & 15));
}

static inline bool isDigit(char ch)
{
	static unsigned short magic = 0x3ff;
	return ((ch >> 4) == 3) && (magic >> (ch & 15));
}

static double toDouble(const char *&str)
{
	const int maxLen = 255;//technically doubles can go til 308+ but whatever
	char temp[maxLen + 1];
	int pos = 0;

	if (*str == ('-')) {
		temp[pos++] = '-';
		++str;
	}
	else if (*str == ('+')) {
		++str;
	}
	while (isDigit(*str) && pos < maxLen) {
		temp[pos++] = *str;
		++str;
	}
	if (*str == ('.') && pos < maxLen) {
		temp[pos++] = '.';
		++str;
	}
	while (isDigit(*str) && pos < maxLen) {
		temp[pos++] = *str;
		++str;
	}
	bool exponent = false;
	if ((*str == ('e') || *str == ('E')) && pos < maxLen) {
		exponent = true;
		temp[pos++] = 'e';
		++str;
		if ((*str == ('-') || *str == ('+')) && pos < maxLen) {
			temp[pos++] = *str;
			++str;
		}
		while (isDigit(*str) && pos < maxLen) {
			temp[pos++] = *str;
			++str;
		}
	}

	temp[pos] = '\0';

	double val;
	if (!exponent && pos < 10) {
		int ival = 0;
		const char *t = temp;
		bool neg = false;
		if (*t == '-') {
			neg = true;
			++t;
		}
		while (*t && *t != '.') {
			ival *= 10;
			ival += (*t) - '0';
			++t;
		}
		if (*t == '.') {
			++t;
			int div = 1;
			while (*t) {
				ival *= 10;
				ival += (*t) - '0';
				div *= 10;
				++t;
			}
			val = ((double)ival) / ((double)div);
		}
		else {
			val = ival;
		}
		if (neg)
			val = -val;
	}
	else {
		//val = QByteArray::fromRawData(temp, pos).toDouble();
		std::stringstream ss(temp);
		ss >> val;
	}
	return val;

}

static inline void parseNumbersArray(const char *&str, std::vector<double> &points)
{
	points.reserve(8);

	while (*str == ' ')
		++str;
	while (isDigit(*str) ||
		*str == ('-') || *str == ('+') ||
		*str == ('.')) {

		points.push_back(toDouble(str));

		while (*str == ' ')
			++str;
		if (*str == (','))
			++str;

		//eat the rest of space
		while (*str == ' ')
			++str;
	}
}

static bool parsePathDataFast(const std::string &dataStr, CHmsPainterPath &path)
{
	double x0 = 0, y0 = 0;              // starting point
	double x = 0, y = 0;                // current point
	char lastMode = 0;
	Vec2 ctrlPt;
	const char *str = dataStr.c_str();
	const char *end = str + dataStr.size();

	while (str != end) {
		while (*str == ' ')
			++str;
		char pathElem = *str;
		++str;
		char endc = *end;
		*const_cast<char *>(end) = 0; // parseNumbersArray requires 0-termination that QStringRef cannot guarantee
		std::vector<double> arg;
		parseNumbersArray(str, arg);
		*const_cast<char *>(end) = endc;
		if (pathElem == ('z') || pathElem == ('Z'))
			arg.push_back(0);//dummy
		const double *num = arg.data();
		int count = arg.size();
		while (count > 0) {
			double offsetX = x;        // correction offsets
			double offsetY = y;        // for relative commands
			switch (pathElem) {
			case 'm': {
				if (count < 2) {
					num++;
					count--;
					break;
				}
				x = x0 = num[0] + offsetX;
				y = y0 = num[1] + offsetY;
				num += 2;
				count -= 2;
				path.moveTo(x0, y0);

				// As per 1.2  spec 8.3.2 The "moveto" commands
				// If a 'moveto' is followed by multiple pairs of coordinates without explicit commands,
				// the subsequent pairs shall be treated as implicit 'lineto' commands.
				pathElem = ('l');
			}
					  break;
			case 'M': {
				if (count < 2) {
					num++;
					count--;
					break;
				}
				x = x0 = num[0];
				y = y0 = num[1];
				num += 2;
				count -= 2;
				path.moveTo(x0, y0);

				// As per 1.2  spec 8.3.2 The "moveto" commands
				// If a 'moveto' is followed by multiple pairs of coordinates without explicit commands,
				// the subsequent pairs shall be treated as implicit 'lineto' commands.
				pathElem = ('L');
			}
					  break;
			case 'z':
			case 'Z': {
				x = x0;
				y = y0;
				count--; // skip dummy
				num++;
				path.closeSubpath();
			}
					  break;
			case 'l': {
				if (count < 2) {
					num++;
					count--;
					break;
				}
				x = num[0] + offsetX;
				y = num[1] + offsetY;
				num += 2;
				count -= 2;
				path.lineTo(x, y);

			}
					  break;
			case 'L': {
				if (count < 2) {
					num++;
					count--;
					break;
				}
				x = num[0];
				y = num[1];
				num += 2;
				count -= 2;
				path.lineTo(x, y);
			}
					  break;
			case 'h': {
				x = num[0] + offsetX;
				num++;
				count--;
				path.lineTo(x, y);
			}
					  break;
			case 'H': {
				x = num[0];
				num++;
				count--;
				path.lineTo(x, y);
			}
					  break;
			case 'v': {
				y = num[0] + offsetY;
				num++;
				count--;
				path.lineTo(x, y);
			}
					  break;
			case 'V': {
				y = num[0];
				num++;
				count--;
				path.lineTo(x, y);
			}
					  break;
			case 'c': {
				if (count < 6) {
					num += count;
					count = 0;
					break;
				}
				Vec2 c1(num[0] + offsetX, num[1] + offsetY);
				Vec2 c2(num[2] + offsetX, num[3] + offsetY);
				Vec2 e(num[4] + offsetX, num[5] + offsetY);
				num += 6;
				count -= 6;
				path.cubicTo(c1, c2, e);
				ctrlPt = c2;
				x = e.x;
				y = e.y;
				break;
			}
			case 'C': {
				if (count < 6) {
					num += count;
					count = 0;
					break;
				}
				Vec2 c1(num[0], num[1]);
				Vec2 c2(num[2], num[3]);
				Vec2 e(num[4], num[5]);
				num += 6;
				count -= 6;
				path.cubicTo(c1, c2, e);
				ctrlPt = c2;
				x = e.x;
				y = e.y;
				break;
			}
			case 's': {
				if (count < 4) {
					num += count;
					count = 0;
					break;
				}
				Vec2 c1;
				if (lastMode == 'c' || lastMode == 'C' ||
					lastMode == 's' || lastMode == 'S')
					c1 = Vec2(2 * x - ctrlPt.x, 2 * y - ctrlPt.y);
				else
					c1 = Vec2(x, y);
				Vec2 c2(num[0] + offsetX, num[1] + offsetY);
				Vec2 e(num[2] + offsetX, num[3] + offsetY);
				num += 4;
				count -= 4;
				path.cubicTo(c1, c2, e);
				ctrlPt = c2;
				x = e.x;
				y = e.y;
				break;
			}
			case 'S': {
				if (count < 4) {
					num += count;
					count = 0;
					break;
				}
				Vec2 c1;
				if (lastMode == 'c' || lastMode == 'C' ||
					lastMode == 's' || lastMode == 'S')
					c1 = Vec2(2 * x - ctrlPt.x, 2 * y - ctrlPt.y);
				else
					c1 = Vec2(x, y);
				Vec2 c2(num[0], num[1]);
				Vec2 e(num[2], num[3]);
				num += 4;
				count -= 4;
				path.cubicTo(c1, c2, e);
				ctrlPt = c2;
				x = e.x;
				y = e.y;
				break;
			}
			case 'q': {
				if (count < 4) {
					num += count;
					count = 0;
					break;
				}
				Vec2 c(num[0] + offsetX, num[1] + offsetY);
				Vec2 e(num[2] + offsetX, num[3] + offsetY);
				num += 4;
				count -= 4;
				path.quadTo(c, e);
				ctrlPt = c;
				x = e.x;
				y = e.y;
				break;
			}
			case 'Q': {
				if (count < 4) {
					num += count;
					count = 0;
					break;
				}
				Vec2 c(num[0], num[1]);
				Vec2 e(num[2], num[3]);
				num += 4;
				count -= 4;
				path.quadTo(c, e);
				ctrlPt = c;
				x = e.x;
				y = e.y;
				break;
			}
			case 't': {
				if (count < 2) {
					num += count;
					count = 0;
					break;
				}
				Vec2 e(num[0] + offsetX, num[1] + offsetY);
				num += 2;
				count -= 2;
				Vec2 c;
				if (lastMode == 'q' || lastMode == 'Q' ||
					lastMode == 't' || lastMode == 'T')
					c = Vec2(2 * x - ctrlPt.x, 2 * y - ctrlPt.y);
				else
					c = Vec2(x, y);
				path.quadTo(c, e);
				ctrlPt = c;
				x = e.x;
				y = e.y;
				break;
			}
			case 'T': {
				if (count < 2) {
					num += count;
					count = 0;
					break;
				}
				Vec2 e(num[0], num[1]);
				num += 2;
				count -= 2;
				Vec2 c;
				if (lastMode == 'q' || lastMode == 'Q' ||
					lastMode == 't' || lastMode == 'T')
					c = Vec2(2 * x - ctrlPt.x, 2 * y - ctrlPt.y);
				else
					c = Vec2(float(x), float(y));
				path.quadTo(c, e);
				ctrlPt = c;
				x = e.x;
				y = e.y;
				break;
			}
			case 'a': {
				if (count < 7) {
					num += count;
					count = 0;
					break;
				}
				double rx = (*num++);
				double ry = (*num++);
				double xAxisRotation = (*num++);
				double largeArcFlag = (*num++);
				double sweepFlag = (*num++);
				double ex = (*num++) + offsetX;
				double ey = (*num++) + offsetY;
				count -= 7;
				double curx = x;
				double cury = y;
				pathArc(path, rx, ry, xAxisRotation, int(largeArcFlag),
					int(sweepFlag), ex, ey, curx, cury);

				x = ex;
				y = ey;
			}
					  break;
			case 'A': {
				if (count < 7) {
					num += count;
					count = 0;
					break;
				}
				double rx = (*num++);
				double ry = (*num++);
				double xAxisRotation = (*num++);
				double largeArcFlag = (*num++);
				double sweepFlag = (*num++);
				double ex = (*num++);
				double ey = (*num++);
				count -= 7;
				double curx = x;
				double cury = y;
				pathArc(path, rx, ry, xAxisRotation, int(largeArcFlag),
					int(sweepFlag), ex, ey, curx, cury);

				x = ex;
				y = ey;
			}
					  break;
			default:
				return false;
			}
			lastMode = pathElem;
		}
	}
	return true;
}

bool CDecodeSvgString::GetSvgPath(const std::string & str, CHmsPainterPath &path)
{
	return parsePathDataFast(str, path);
}

bool CDecodeSvgString::GetSvgPolygon(const std::string & str, std::vector<Vec2> & vOut)
{
	bool bRet = false;
	std::stringstream ss(str);

	while (!ss.eof())
	{
		char c = 0;
		Vec2 pos;
		ss >> pos.x;
		ss >> c;
		ss >> pos.y;
		if (c == ',' && !ss.fail())
		{
			pos.y = -pos.y;
			vOut.push_back(pos);
		}
		else
		{
			break;
		}
	}

	if (vOut.size())
	{
		bRet = true;
	}
	return bRet;
}
NS_HMS_END