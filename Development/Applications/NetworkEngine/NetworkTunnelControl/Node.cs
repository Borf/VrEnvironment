using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace NetworkTunnelControl
{
	class Node
	{
		public string name { get; set; }
		public string guid { get; set; }

		[TypeConverter(typeof(ExpandableObjectConverter))]
		public List<Component> components { get; set; } = new List<Component>();
	}
}
