using System;


namespace ICSproj.BL.Models
{
    public class PhotoDetailModel : ModelBase
    {
        public byte[] Photo { get; set; }
        public Guid ForeignGuid { get; set; }
    }
}
