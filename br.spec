# -*- mode: python ; coding: utf-8 -*-


a = Analysis(
    ['br.py'],
    pathex=[],
    binaries=[],
    datas=[('ferramentas', 'ferramentas')],
    hiddenimports=['json', 'base64', 'csv', 'datetime', 'hashlib', 'io', 'platform', 're', 'secrets', 'sqlite3', '_sqlite3', 'unicodedata', 'urllib.parse', 'urllib.request', 'uuid', 'pygame'],
    hookspath=[],
    hooksconfig={},
    runtime_hooks=[],
    excludes=[],
    noarchive=False,
    optimize=0,
)
pyz = PYZ(a.pure)

exe = EXE(
    pyz,
    a.scripts,
    a.binaries,
    a.datas,
    [],
    name='br',
    debug=False,
    bootloader_ignore_signals=False,
    strip=False,
    upx=False,
    upx_exclude=[],
    runtime_tmpdir=None,
    console=True,
    disable_windowed_traceback=False,
    argv_emulation=False,
    target_arch=None,
    codesign_identity=None,
    entitlements_file=None,
)
