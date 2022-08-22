using System;


namespace ICSproj.BL.Models
{
    public class PhotoListModel : ModelBase
    {
        public byte[] Photo { get; set; }
        public Guid ForeignGuid { get; set; }
    }
}
