using System.Collections.Generic;


namespace ICSproj.BL.Models
{
    public class StageListModel : ModelBase
    {
        public string Name { get; set; }
        public string Description { get; set; }
        public List<ScheduleListModel> Schedule { get; set; }
    }
}
