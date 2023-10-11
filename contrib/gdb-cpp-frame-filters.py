import re
import pdb
from collections import deque

from gdb.FrameDecorator import FrameDecorator

class CollapseCPPFilter:
    COLLAPSE_REGEX = r'^std::_'
    def __init__(self):
        self.name = "collapse_cpp_filter"
        self.enabled = True
        self.priority = 100
        gdb.frame_filters[self.name] = self

    def should_collapse(self, frame):
        if not isinstance(frame.function(), str):
            return False
        return re.match(CollapseCPPFilter.COLLAPSE_REGEX, frame.function())

    def filter(self, frame_iter):
        def get_frame():
            real_frame = None
            try:
                real_frame = next(frame_iter)
            except StopIteration:
                return None
            if real_frame is None:
                return None
            return FrameDecorator(real_frame)

        queue = deque()
        frame = get_frame()
        if frame is None:
            return

        while frame is not None:
            while frame is not None and self.should_collapse(frame):
                queue.append(frame)
                frame = get_frame()

            if frame is not None and not self.should_collapse(frame):
                frame.elided_frames = list(queue)
                queue.clear()
                yield frame
                frame = get_frame()

        for frame in queue:
            yield frame


CollapseCPPFilter()
