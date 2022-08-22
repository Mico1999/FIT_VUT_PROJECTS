using ICSproj.BL.Models;

namespace ICSproj.App.Messages
{
    public class SelectedMessage<T> : Message<T>
        where T : IModel
    {
    }
}
