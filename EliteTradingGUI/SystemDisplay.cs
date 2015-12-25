using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace EliteTradingGUI
{
    public partial class SystemDisplay : Form
    {
        private readonly Dictionary<ulong, PointF> _points;
        private RectangleF[] _rectangles;
        private Graphics _graphics;
        private Pen _arrowPen;
        private Func<PointF, PointF> _transform;
        private Func<PointF, PointF> _reverseTransform;
        private RectangleF _bounds;
        private RectangleF _boundsWithMargin;
        private readonly CoordinateBounds _pointBounds;
        private List<RouteNode> _route;
        private SolidBrush _redBrush;
        private SolidBrush _blackBrush;

        class CoordinateBounds
        {
            public PointF Min, Max;

            public CoordinateBounds(IEnumerable<PointF> points)
            {
                bool set = false;
                foreach (var point in points)
                {
                    if (!set)
                    {
                        Min.X = point.X;
                        Max.X = point.X;
                        Min.Y = point.Y;
                        Max.Y = point.Y;
                        set = true;
                    }
                    else
                    {
                        Min.X = Math.Min(Min.X, point.X);
                        Max.X = Math.Max(Max.X, point.X);
                        Min.Y = Math.Min(Min.Y, point.Y);
                        Max.Y = Math.Max(Max.Y, point.Y);
                    }
                }
            }

            public float Width
            {
                get { return Max.X - Min.X; }
            }

            public float Height
            {
                get { return Max.Y - Min.Y; }
            }
        }

        public SystemDisplay(Dictionary<ulong, SystemPoint> points, RouteNode node)
        {
            InitializeComponent();
            _points = points.ToDictionary(x => x.Key, x => new PointF((float)x.Value.X, (float)x.Value.Z));

            _route = node.ToList();
            if (_route.Count == 0)
                return;
            _pointBounds = new CoordinateBounds(_route.Select(x => _points[x.SystemId]));
            _bounds = new RectangleF(0, 0, ClientRectangle.Width, ClientRectangle.Height);
            _boundsWithMargin = GetBoundsWithMargin(_bounds);
            var tuple = GetTransform(_bounds, _boundsWithMargin, _pointBounds);
            _transform = tuple.Item1;
            _reverseTransform = tuple.Item2;

            _graphics = Graphics.FromHwnd(Handle);
            _arrowPen = new Pen(Color.Red);
            var capPath = new GraphicsPath();
            float multiplier = 2;
            float length = 8;
            float width = 2;
            capPath.AddLine( 0 * multiplier,  0 * multiplier,  width * multiplier, -length * multiplier);
            capPath.AddLine( width * multiplier, -length * multiplier, -width * multiplier, -length * multiplier);
            capPath.AddLine(-width * multiplier, -length * multiplier,  0 * multiplier,  0 * multiplier);
            _arrowPen.CustomEndCap = new CustomLineCap(null, capPath);
            _blackBrush = new SolidBrush(Color.Black);
            _redBrush = new SolidBrush(Color.Red);
        }

        private RectangleF GetBoundsWithMargin(RectangleF bounds, int margin = 20)
        {
            return new RectangleF(bounds.X + margin, bounds.Y + margin, bounds.Width - margin * 2, bounds.Height - margin * 2);
        }

        private Tuple<Func<PointF, PointF>, Func<PointF, PointF>> GetTransform(RectangleF bounds, RectangleF boundsWithMargin, CoordinateBounds pointBounds)
        {
            float multx = boundsWithMargin.Width/(pointBounds.Max.X - pointBounds.Min.X);
            float multy = boundsWithMargin.Height / (pointBounds.Max.Y - pointBounds.Min.Y);
            multx = multy = Math.Min(multx, multy);
            var width = pointBounds.Width * multx;
            var height = pointBounds.Height * multy;
            float sumx = boundsWithMargin.X - pointBounds.Min.X * multx + (boundsWithMargin.Width - width) / 2;
            float sumy = boundsWithMargin.Y - pointBounds.Min.Y * multy + (boundsWithMargin.Height - height) / 2;

            float divx = 1 / multx;
            float divy = 1 / multy;
            float subx = -sumx * divx;
            float suby = -sumy * divy;

            Func<PointF, PointF> forward = p => new PointF(p.X * multx + sumx, p.Y * multy + sumy);
            Func<PointF, PointF> backward = p => new PointF(p.X * divx + subx, p.Y * divy + suby);
            return new Tuple<Func<PointF, PointF>, Func<PointF, PointF>>(forward, backward);
        }

        private RectangleF ReverseTransform(RectangleF rect)
        {
            var topleft = new PointF(rect.X, rect.Y);
            var bottomright = new PointF(rect.X + rect.Width, rect.Y + rect.Height);
            topleft = _reverseTransform(topleft);
            bottomright = _reverseTransform(bottomright);
            return new RectangleF(topleft.X, topleft.Y, bottomright.X - topleft.X, bottomright.Y - topleft.Y);
        }

        private void DrawCircle(Brush brush, PointF center, float radius)
        {
            _graphics.FillEllipse(brush, center.X - radius, center.Y - radius, radius * 2, radius * 2);
        }

        private void SystemDisplay_Paint(object sender, PaintEventArgs e)
        {
            using (var font = new Font("Tahoma", 10))
            {
                int index = 0;
                var bounds = ReverseTransform(_bounds);
                for (int i = 0; i < _route.Count; i++)
                {
                    var node = _route[i];
                    var point = _transform(_points[node.SystemId]);
                    DrawCircle(_redBrush, point, 4);
                    if (i > 0)
                    {
                        //for (int j = 1; j < node.HopRoute.Count; j++)
                        //{
                        //    var hop = node.HopRoute[j];
                        //    var previousHop = node.HopRoute[j - 1];
                        //    var A = _transform(_points[previousHop]);
                        //    var B = _transform(_points[hop]);
                        //    _graphics.DrawLine(_arrowPen, A, B);
                        //    _graphics.DrawString(index.ToString(), font, _redBrush, new PointF(B.X, B.Y - 20));
                        //    index++;
                        //}
                        _graphics.DrawLine(_arrowPen, _transform(_points[_route[i - 1].SystemId]), point);
                    }
                    else
                    {
                        _graphics.DrawString(index.ToString(), font, _redBrush, new PointF(point.X, point.Y - 20));
                        index++;
                    }
                }
                //foreach (var point in _points.Values.Where(x => bounds.Contains(x)).Select(p => _transform(p)))
                //    DrawCircle(_blackBrush, point, 2);
            }
        }

        private void SystemDisplay_SizeChanged(object sender, EventArgs e)
        {
            _graphics.Dispose();
            _graphics = Graphics.FromHwnd(Handle);
            _bounds = new RectangleF(0, 0, ClientRectangle.Width, ClientRectangle.Height);
            _boundsWithMargin = GetBoundsWithMargin(_bounds);
            var tuple = GetTransform(_bounds, _boundsWithMargin, _pointBounds);
            _transform = tuple.Item1;
            _reverseTransform = tuple.Item2;
            Refresh();
        }
    }
}
