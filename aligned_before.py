from construct import *

def evaluate(param, context):
    return param(context) if callable(param) else param

class AlignedBefore(Subconstruct):
    def __init__(self, modulus, subcon, pattern=b"\x00"):
        if not isinstance(pattern, bytes) or len(pattern) != 1:
            raise PaddingError("pattern expected to be bytes character")
        super().__init__(subcon)
        self.modulus = modulus
        self.pattern = pattern

    def _parse(self, stream, context, path):
        modulus = evaluate(self.modulus, context)
        if modulus < 2:
            raise PaddingError("expected modulo 2 or greater", path=path)
        position = stream_tell(stream, path)
        pad = (-position) % modulus
        stream_read(stream, pad, path)
        return self.subcon._parsereport(stream, context, path)

    def _build(self, obj, stream, context, path):
        modulus = evaluate(self.modulus, context)
        if modulus < 2:
            raise PaddingError("expected modulo 2 or greater", path=path)
        position = stream_tell(stream, path)
        pad = (-position) % modulus
        stream_write(stream, self.pattern * pad, pad, path)
        return self.subcon._build(obj, stream, context, path)

    def _sizeof(self, context, path):
        try:
            modulus = evaluate(self.modulus, context)
            if modulus < 2:
                raise PaddingError("expected modulo 2 or greater", path=path)
            subconlen = self.subcon._sizeof(context, path)
            return subconlen + (-subconlen % modulus)
        except (KeyError, AttributeError):
            raise SizeofError("cannot calculate size, key not found in context", path=path)
