using System;

using System.Drawing;
using System.Net.Mime;
using System.Text;

namespace ICSproj.DAL.Entities
{
    public class PhotoEntity : BaseEntity
    {
        public PhotoEntity(Guid id)
        {
            Id = id;
        }
        public PhotoEntity()
        { }

        public byte[] Photo { get; set; }
        public Guid ForeignGuid{ get; set; }
    }
}
